#include "backtrack.h"
#include "helpers/math.hpp"
#include "helpers/utils.hpp"
#include "options.hpp"
#include "autowall.h"

#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / g_GlobalVars->interval_per_tick ) )
#define TICKS_TO_TIME( t )		( g_GlobalVars->interval_per_tick *( t ) )

backtrack_aim_data c_backtracking::get_best_backtrack_aim_tick(CUserCmd * cmd, float best_fov)
{
	C_BasePlayer* best_entity = nullptr;
	int best_tick = cmd->tick_count;
	QAngle best_aim_angle = cmd->viewangles;
	bool better_targ = false;
	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

		if (!entity || entity == g_LocalPlayer || !entity->is_alive() || entity->is_dormant())
			continue;

		for (auto& record : entity_records[i])
		{
			auto backtracked_hitbox_pos = entity->get_backtracked_hitbox_pos(g_vars.aimbot_hitbox, record.ent_matrix);
			QAngle angle = Math::CalcAngle(g_LocalPlayer->get_eye_pos(), backtracked_hitbox_pos);
			float fov = Math::GetFOV(cmd->viewangles, angle);

			if (fov > best_fov || fov > g_vars.aimbot_fov)
				continue;

			if (g_vars.aimbot_vis_check)
			{

				Ray_t ray;
				CGameTrace trace;
				CTraceFilter filter;

				filter.pSkip = g_LocalPlayer;
				ray.Init(g_LocalPlayer->get_eye_pos(), backtracked_hitbox_pos);

				g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &trace);
				if (trace.DidHit()) //continue if position is behind wall
				{
					continue;
				}
			}
			if (g_vars.aimbot_smoke_check)
			{
				const bool smoked = Utils::line_goes_through_smoke(g_LocalPlayer->get_eye_pos(), backtracked_hitbox_pos);
				if (smoked)
					continue;
			}

			best_fov = fov;
			best_tick = record.tick;
			cmd->tick_count = best_tick;
			best_aim_angle = angle;
			better_targ = true;
			best_entity = entity;
		}
	}

	return { best_entity, best_tick, best_aim_angle, better_targ };
}

void c_backtracking::register_tick(CUserCmd * cmd)
{
	if (!g_vars.backtrack_enabled)
		return;

	if (!g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
		return;

	if (!cmd)
		return;

	if (!g_LocalPlayer)
		return;

	const int backtrack_ticks = g_vars.backtrack_ticks > 0 ? g_vars.backtrack_ticks : TIME_TO_TICKS(.2);

	for (size_t i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

		if (!entity)
			continue;

		auto& current_records = entity_records[entity->EntIndex()];

		//pushes out oldest records, at front
		while (current_records.size() > backtrack_ticks)
			current_records.pop_front();

		if (!entity->is_alive())
			continue;

		if (entity->is_dormant() || entity->m_bGunGameImmunity())
			continue;

		if (!player_matrixes[entity->EntIndex()].matrix)
			continue;

		//puts newest records at back
		entity_records[entity->EntIndex()].emplace_back(backtrack_record{ entity->get_bone_pos(8), entity->m_vecOrigin(), player_matrixes[entity->EntIndex()], cmd->tick_count });
	}
}

void c_backtracking::clean_records(CUserCmd* cmd)
{
	const int backtrack_ticks = g_vars.backtrack_ticks > 0 ? g_vars.backtrack_ticks : TIME_TO_TICKS(.2);

	for (size_t i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

		if (!entity)
			continue;

		auto& current_records = entity_records[entity->EntIndex()];

		if (!entity->is_alive())
		{
			//fixing some visual issues
			if (!current_records.empty())
				current_records.clear();

			continue;
		}

		for (int v = 0; v < current_records.size(); v++)
		{
			auto& record = current_records.at(v);
			if (cmd->tick_count - record.tick > backtrack_ticks)
				current_records.erase(current_records.begin() + v);
		}
	}
}

void c_backtracking::Begin(CUserCmd * cmd)
{
	if (!cmd)
		return;

	if (!g_vars.backtrack_enabled)
		return;

	if (g_vars.backtrack_on_key > 0 && !GetAsyncKeyState(g_vars.backtrack_on_key))
		return;

	best_entity = nullptr;
	C_BasePlayer* localPlayer = g_LocalPlayer;

	if (!localPlayer)
		return;

	if (!localPlayer->is_alive())
		return;

	const float server_time = localPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
	auto weapon = (C_BaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(localPlayer->m_hActiveWeapon());

	if (!weapon)
		return;

	const auto restore_record = [](C_BasePlayer* entity, CUserCmd* cmd, matrixes best_mtrx, int tick_count) -> void
	{
		etc_player_info::backtrack_tick[entity->EntIndex()] = cmd->tick_count - tick_count;
		uintptr_t* bonecache = *(uintptr_t**)((uintptr_t)entity + 0x28FC); //0x2900 - 4
		const uint32_t count = *(uintptr_t*)((uintptr_t)entity + 0x2908); // 0x290C - 4
		memcpy(bonecache, best_mtrx.matrix, sizeof(matrix3x4_t) * (count < 256 ? count : 256));
		cmd->tick_count = tick_count;
	};

	const auto dist = [=](const Vector a, const Vector b) -> float
	{
		return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
	};

	if (!(cmd->buttons & IN_ATTACK) && !(cmd->buttons & IN_ATTACK2))
		return;

	if (weapon->m_flNextPrimaryAttack() > server_time)
		return;

	if (!(weapon->is_knife() || weapon->is_taser()))
	{
		float fov = FLT_MAX;
		int tick_count = 0;
		bool has_target = false;

		for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
		{
			C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

			if (!entity || !entity->is_alive() || entity->is_dormant())
				continue;

			for (auto& record : entity_records[i])
			{
				const auto get_lowest_fov = [=](matrixes m, C_BasePlayer* entity) -> float
				{
					float lowest_fov = FLT_MAX;

					if (!entity)
						return lowest_fov;

					if (!entity->is_alive() || entity->is_dormant())
						return lowest_fov;

					for (size_t h = 0; h < Hitboxes::HITBOX_MAX; h++)
					{
						auto hitbox_pos = entity->get_backtracked_hitbox_pos(h, m);

						if (hitbox_pos == Vector{})
							continue;

						auto ang = Math::CalcAngle(g_LocalPlayer->get_eye_pos(), hitbox_pos);
						const auto temp_fov = Math::GetFOV(cmd->viewangles, ang);

						if (temp_fov < lowest_fov)
							lowest_fov = temp_fov;
					}
					return lowest_fov;
				};

				const float tmp_fov = get_lowest_fov(record.ent_matrix, entity);

				//prevent backtracking lagcomp breakers
				const float origin_dist = (entity->m_vecOrigin() - record.origin).Length2D();

				if (origin_dist >= 64 && g_vars.backtrack_lc_break)
					continue;

				if (origin_dist > g_vars.backtrack_dist)
					continue;

				if (tmp_fov < fov)
				{
					fov = tmp_fov;
					tick_count = record.tick;
					has_target = true;
					best_entity = entity;
					best_matrix = record.ent_matrix;
				}
			}

		}

		if (best_entity && has_target)
			restore_record(best_entity, cmd, best_matrix, tick_count);
	}
	else
	{
		//32f stab, 48f slash, 183 taser dist
		float distance = weapon->is_knife() ? cmd->buttons & IN_ATTACK ? 32.f : 48.f : 183.f;
		int tick_count = 0;
		bool has_target = false;

		for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
		{
			C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

			if (!entity || !entity->is_alive() || entity->is_dormant())
				continue;

			for (auto& record : entity_records[i])
			{
				const float tmp_dist = dist(g_LocalPlayer->m_vecOrigin(), record.origin);

				if (tmp_dist <= distance)
				{
					distance = tmp_dist;
					tick_count = record.tick;
					has_target = true;
					best_entity = entity;
					best_matrix = record.ent_matrix;
				}
			}
		}

		if (best_entity && has_target)
			restore_record(best_entity, cmd, best_matrix, tick_count);
	}
}

void c_backtracking::End()
{
	if (!g_vars.backtrack_enabled)
		return;

	if (g_vars.backtrack_on_key > 0 && !GetAsyncKeyState(g_vars.backtrack_on_key))
		return;

	best_entity = nullptr;
}

void c_backtracking::get_best_hvh_backtrack_tick(C_BasePlayer * entity, CUserCmd* cmd)
{
	if (!cmd)
		return;

	if (!entity)
		return;

	if (!g_vars.aimbot_enabled || !g_vars.backtrack_enabled)
		return;

	for (const auto record : entity_records[entity->EntIndex()])
	{

		if (cmd->tick_count - record.tick > g_vars.backtrack_ticks)
			continue;

		if ((entity->m_vecOrigin() - record.origin).Length2D() > g_vars.backtrack_dist)
			continue;

		auto bt_hitbox_pos = entity->get_backtracked_hitbox_pos(g_vars.aimbot_hitbox, record.ent_matrix);

//		auto autowall_dmg = Autowall::Get().can_hit(bt_hitbox_pos);

//		if (autowall_dmg >= g_vars.aimbot_autowall_dmg)
		{
			etc_player_info::backtrack_tick[entity->EntIndex()] = cmd->tick_count - record.tick;
			cmd->tick_count = record.tick;

			auto angle = Math::CalcAngle(g_LocalPlayer->get_eye_pos(), bt_hitbox_pos);
			if (g_vars.misc_rcs)
			{
				angle -= g_LocalPlayer->m_aimPunchAngle() * g_vars.misc_rcs_amount;
			}
			cmd->viewangles = angle;
			Math::Clamp(cmd->viewangles);
		}
	}
}

bool c_backtracking::is_over_player_backtrack_record(CUserCmd* cmd)
{
	auto data = g_LocalPlayer->m_hActiveWeapon()->get_cs_wpn_data();

	Vector start = g_LocalPlayer->get_eye_pos();
	Vector end;

	Math::AngleVectors(cmd->viewangles + (g_vars.misc_rcs ? QAngle(0,0,0) : g_LocalPlayer->m_aimPunchAngle()), end);
	end = start + (end * data->range);

	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

		if (!entity || !entity->is_alive() || entity->is_dormant() || !entity->GetModel())
			continue;

		if (entity == g_LocalPlayer)
			continue;

		if (entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum() && !g_vars.triggerbot_targets[0])
			continue;

		if (entity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum() && !g_vars.triggerbot_targets[1])
			continue;

		auto hdr = g_MdlInfo->GetStudiomodel(entity->GetModel());

		if (!hdr)
			continue;

		auto set = hdr->GetHitboxSet(0).value();

		if (!set)
			continue;

		for (auto& record : entity_records[i])
		{
			if (cmd->tick_count - record.tick > g_vars.backtrack_ticks)
				continue;

			for (size_t h{}; h < set->numhitboxes; ++h)
			{
				auto hitbox = set->GetHitbox(h).value();
				if (!hitbox || hitbox->m_flRadius == -1.f) 
					continue;

				constexpr auto hitbox_scale = .80;
				
				Vector mins, maxs;
				Math::VectorTransform(hitbox->bbmin * hitbox_scale, record.ent_matrix.matrix[hitbox->bone], mins); //* .80 is for hitbox scaling - less inaccuracies
				Math::VectorTransform(hitbox->bbmax * hitbox_scale, record.ent_matrix.matrix[hitbox->bone], maxs);

				auto intersection_opt = get_intersection_point(start, end, mins, maxs, hitbox->m_flRadius * hitbox_scale);

				if (!intersection_opt.has_value())
					return false;

				if (auto intersection = intersection_opt.value(); intersection.IsValid() && intersection != Vector(0, 0, 0))
				{
					Ray_t ray;
					CGameTrace trace;
					CTraceFilter filter;

					filter.pSkip = g_LocalPlayer;
					ray.Init(start, intersection);

					g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &trace);
					if (!trace.DidHit()) //position isn't behind wall
					{
						etc_player_info::backtrack_tick[entity->EntIndex()] = cmd->tick_count - record.tick;
						cmd->tick_count = record.tick;
						return true;
					}
				}
			}
		}
	}
	return false;
}

void c_backtracking::draw_backtrack_skeleton(C_BasePlayer* entity)
{
	if (!entity->GetModel())
		return;

	auto draw_bone_line = [=](int bone, int bone_parent, matrixes m) -> void
	{
		Vector bone_pos;
		Vector bone_parent_pos;

		if (!(Math::WorldToScreen(entity->get_backtracked_bone_pos(bone, m), bone_pos)))
			return;

		if (!(Math::WorldToScreen(entity->get_backtracked_bone_pos(bone_parent, m), bone_parent_pos)))
			return;
		auto get_team = [=]() -> int
		{
			if (entity == g_LocalPlayer)
				return 2;
			if (entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
				return 0;

			return 1;
		};

		g_VGuiSurface->DrawSetColor(g_vars.color_backtrack_skeleton[get_team()]);
		g_VGuiSurface->DrawLine(bone_pos.x, bone_pos.y, bone_parent_pos.x, bone_parent_pos.y);
	};

	studiohdr_t* studio_model = g_MdlInfo->GetStudiomodel(entity->GetModel());

	if (!studio_model)
		return;
	for (const auto record : entity_records[entity->EntIndex()])
	{
		if (g_GlobalVars->tickcount - record.tick > g_vars.backtrack_ticks)
			continue;

		for (int i = 0; i < studio_model->numbones; ++i)
		{
			mstudiobone_t* bone = studio_model->GetBone(i).value();
			if (!bone || !(bone->flags & BONE_USED_BY_HITBOX) || bone->parent == -1)
				continue;

			draw_bone_line(i, bone->parent, record.ent_matrix);
		}
	}
}

std::optional<Vector> c_backtracking::get_intersection_point(const Vector& start, const Vector& end, const Vector& mins, const Vector& maxs, float radius)
{
	const static auto sphere_ray_intersection = [start, end, radius](const Vector& center) -> std::optional<Vector>
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
		if (auto intersection = sphere_ray_intersection(mins + delta * float(i)); intersection.has_value() && intersection.value().IsValid())
			return intersection;
	}

	if (auto intersection = sphere_ray_intersection(maxs); intersection.has_value() && intersection.value().IsValid())
		return intersection;

	return {};
}

c_backtracking backtrack = c_backtracking();