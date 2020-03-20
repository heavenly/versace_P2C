#include "resolver.h"
#include "helpers/math.hpp"
#include "local_info.h"
#include "notification.h"
#include "options.hpp"

c_resolver resolver = c_resolver();

void c_resolver::log_player_info()
{
	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

		if (!entity)
			continue;

		if (!entity->is_alive() || entity->is_dormant())
			continue;

		if (entity->m_vecVelocity().Length2D() > 0)
			player_res_info[entity->EntIndex()].moving_lby = entity->m_flLowerBodyYawTarget();
	}
}

void c_resolver::on_player_hurt(C_BasePlayer * entity)
{
	player_res_info[entity->EntIndex()].shots_missed--;
	info_lp::player_hurt_triggered = true;
}

void c_resolver::on_weapon_fire(C_BasePlayer * entity)
{
	info_lp::weapon_fire_triggered = true;
}

Vector c_resolver::get_intersection_point(const Vector & start, const Vector & end, const Vector & mins, const Vector & maxs, float radius)
{
	auto sphere_ray_intersection = [start, end, radius](const Vector & center) -> Vector
	{
		auto direction = (end - start).Normalized();

		auto q = center - start;
		auto v = q.Dot(direction);
		auto d = radius * radius - (q.LengthSqr() - v * v);

		if (d < FLT_EPSILON)
			return {};

		return start + direction * (v - std::sqrt(d));
	};

	auto delta = (maxs - mins).Normalized();
	for (size_t i{}; i < std::floor(mins.DistTo(maxs)); ++i)
	{
		if (auto intersection = sphere_ray_intersection(mins + delta * float(i)); intersection.IsValid())
			return intersection;
	}

	if (auto intersection = sphere_ray_intersection(maxs); intersection.IsValid())
		return intersection;

	return {};
}

void c_resolver::on_bullet_impact(Vector impact_pos)
{
	if (!g_vars.aimbot_autoshoot_enabled || !g_vars.misc_resolver || info_lp::player_hurt_triggered || !info_lp::weapon_fire_triggered)
		return;

	C_BasePlayer * entity = (C_BasePlayer*)g_EntityList->GetClientEntity(info_lp::current_target_index);

	if (!entity)
		return;

	auto hdr = g_MdlInfo->GetStudiomodel(entity->GetModel());

	if (!hdr)
		return;

	auto set = hdr->GetHitboxSet(0).value();

	if (!set)
		return;

	info_lp::bullet_impact_triggered = true;

	for (size_t i{}; i < set->numhitboxes; ++i)
	{
		auto hitbox = set->GetHitbox(i).value();
		if (!hitbox || hitbox->m_flRadius == -1.f)
			continue;

		Vector mins, maxs;
		Math::VectorTransform(hitbox->bbmin, player_matrixes[entity->EntIndex()].matrix[hitbox->bone], mins);
		Math::VectorTransform(hitbox->bbmax, player_matrixes[entity->EntIndex()].matrix[hitbox->bone], maxs);

		if (auto intersection = get_intersection_point(g_LocalPlayer->get_eye_pos(), impact_pos, mins, maxs, hitbox->m_flRadius); intersection.IsValid())
		{
			//missed due to bad resolve
			player_res_info[entity->EntIndex()].shots_missed++;
			push_notifications::Push("missed due to bad resolve");
			return;
		}
	}
	push_notifications::Push("missed due to spread");
}

void c_resolver::do_resolver()
{
	//call in fsn pdu start
	if (!g_vars.misc_resolver)
		return;

	for (size_t i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
		if (!entity)
			continue;

		if (!entity->is_alive() || entity->is_dormant())
			continue;

		entity->m_angEyeAngles().yaw = (entity->m_vecVelocity().Length2D() > 0) ? get_moving_yaw(entity) : get_standing_yaw(entity);
	}
}

float c_resolver::get_standing_yaw(C_BasePlayer * player)
{
	auto get_max_delta = [=]() -> float
	{
		const auto animstate = uintptr_t(player->get_base_player_animstate());

		const float duck_amount = *(float*)(animstate + 0xA4);
		const float speed_fraction = std::max(0.f, std::min(*reinterpret_cast<float*>(animstate + 0xF8), 1.f));

		const float speed_factor = std::max(0.f, std::min(1.f, *reinterpret_cast<float*> (animstate + 0xFC)));

		const float unk1 = ((*reinterpret_cast<float*> (animstate + 0x11C) * -0.30000001) - 0.19999999) * speed_fraction;
		float unk2 = unk1 + 1.f;

		if (duck_amount > 0)
			unk2 += ((duck_amount * speed_factor) * (0.5f - unk2));

		const float unk3 = *(float*)(animstate + 0x334) * unk2;
		return unk3;
	};

	auto current_resolver_info = player_res_info[player->EntIndex()];

	float lby = player->m_flLowerBodyYawTarget(); //fake, unless shitty desync
	float eye_angs = player->m_angEyeAngles().yaw; //mostly fake

	const std::vector<float> magic_numbers = { 0, get_max_delta(), -get_max_delta(), 114, -114, 156, -156 };
	auto current_shots = current_resolver_info.shots_missed % magic_numbers.size();
	return (lby + magic_numbers.at(current_shots));
}

float c_resolver::get_moving_yaw(C_BasePlayer * player)
{
	return player->m_flLowerBodyYawTarget();
}