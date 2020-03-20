#include "autowall.h"
#include "helpers/math.hpp"
#include "options.hpp"

#define DAMAGE_NO		0
#define DAMAGE_EVENTS_ONLY	1
#define DAMAGE_YES		2
#define DAMAGE_AIM		3

void ScaleDamage_1(int hitgroup, C_BasePlayer* enemy, float weapon_armor_ratio, float& current_damage)
{
	int armor = enemy->m_ArmorValue();
	float ratio;

	switch (hitgroup)
	{
	case HITGROUP_HEAD:
		current_damage *= 4.f;
		break;
	case HITGROUP_STOMACH:
		current_damage *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		current_damage *= 0.75f;
		break;
	}

	if (armor > 0)
	{
		switch (hitgroup)
		{
		case HITGROUP_HEAD:
			if (enemy->m_bHasHelmet())
			{
				ratio = (weapon_armor_ratio * 0.5) * current_damage;
				if (((current_damage - ratio) * 0.5) > armor)
				{
					ratio = current_damage - (armor * 2.0);
				}
				current_damage = ratio;
			}
			break;
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			ratio = (weapon_armor_ratio * 0.5) * current_damage;
			if (((current_damage - ratio) * 0.5) > armor)
			{
				ratio = current_damage - (armor * 2.0);
			}
			current_damage = ratio;
			break;
		}
	}
}

void c_autowall::trace_line(Vector& abs_start, Vector& abs_end, unsigned int mask, IClientEntity* ignore, CGameTrace* ptr)
{
	Ray_t ray;
	ray.Init(abs_start, abs_end);
	CTraceFilter filter;
	filter.pSkip = ignore;

	g_EngineTrace->TraceRay(ray, mask, &filter, ptr);
}

void c_autowall::clip_trace_to_players(const Vector& abs_start, const Vector abs_end, unsigned int mask,
                                     ITraceFilter* filter, CGameTrace* tr)
{
	if (!g_LocalPlayer || !g_LocalPlayer->is_alive())
		return;

	C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon();

	if (!weapon)
		return;

	if (weapon->is_grenade() || weapon->is_knife())
		return;

	auto dist_to_ray = [](const Vector& pos, const Vector& ray_start, const Vector& ray_end, float* along = nullptr,
	                      Vector* point_on_ray = nullptr) -> float
	{
		Vector to = pos - ray_start;
		Vector direction = ray_end - ray_start;
		float length = direction.Normalized().Length();

		float range_along = direction.Dot(to);
		if (along)
			*along = range_along;

		float range;

		if (range_along < 0.0f)
		{
			// off start point
			range = -(pos - ray_start).Length();

			if (point_on_ray)
				*point_on_ray = ray_start;
		}
		else if (range_along > length)
		{
			// off end point
			range = -(pos - ray_end).Length();

			if (point_on_ray)
				*point_on_ray = ray_end;
		}
		else // within ray bounds
		{
			//Vector on_ray = ray_start + (direction.operator*(range_along));
			Vector on_ray = ray_start + (direction * range_along);
			range = (pos - on_ray).Length();

			if (point_on_ray)
				*point_on_ray = on_ray;
		}

		return range;
	};

	CGameTrace player_trace;
	Ray_t ray;
	float smallest_fraction = tr->fraction;
	constexpr float max_range = 60.0f;

	ray.Init(abs_start, abs_end);

	for (int k = 1; k <= g_EngineClient->GetMaxClients(); ++k)
	{
		C_BasePlayer* player = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(k));

		if (!player || !player->is_alive())
			continue;

		if (player->is_dormant())
			continue;

		if (filter && !(filter->ShouldHitEntity(static_cast<IHandleEntity*>(player), mask)))
			continue;

		const float range = dist_to_ray(player->world_space_center(), abs_start, abs_end);

		if (range < 0.0f || range > max_range)
			continue;

		g_EngineTrace->ClipRayToEntity(ray, mask | CONTENTS_HITBOX, static_cast<IHandleEntity*>(player), &player_trace);
		if (player_trace.fraction < smallest_fraction)
		{
			*tr = player_trace;
			smallest_fraction = player_trace.fraction;
		}
	}
}

bool c_autowall::breakable_entity(IClientEntity* entity)
{
	if (!entity || !entity->EntIndex())
		return false;

	// backup original take_damage value.

	const int take_damage = *reinterpret_cast<int *>(uintptr_t(entity) + 0x280);

	const ClientClass* client_class = entity->GetClientClass();

	if (client_class->m_pNetworkName[1] == 'B' && client_class->m_pNetworkName[9] == 'e' && client_class->m_pNetworkName
		[10] == 'S' && client_class->m_pNetworkName[16] == 'e')
		*reinterpret_cast<int *>(uintptr_t(entity) + 0x280) = DAMAGE_YES;

	else if (client_class->m_pNetworkName[1] != 'B' && client_class->m_pNetworkName[5] != 'D')
		*reinterpret_cast<int *>(uintptr_t(entity) + 0x280) = DAMAGE_YES;

	else if (client_class->m_pNetworkName[1] != 'F' && client_class->m_pNetworkName[4] != 'c' && client_class->
		m_pNetworkName[5] != 'B' && client_class->m_pNetworkName[9] != 'h') // CFuncBrush
		*reinterpret_cast<int *>(uintptr_t(entity) + 0x280) = DAMAGE_YES;

	using IsBreakableEntity_t = bool(__thiscall *)(IClientEntity*);
	static IsBreakableEntity_t IsBreakableEntityEx = nullptr;

	if (!IsBreakableEntityEx)
		IsBreakableEntityEx = reinterpret_cast<IsBreakableEntity_t>(Utils::PatternScan(
			"client_panorama.dll", "55 8B EC 51 56 8B F1 85 F6 74 68").value());

	*reinterpret_cast<int *>(uintptr_t(entity) + 0x280) = take_damage;

	return IsBreakableEntityEx(entity);
}

void c_autowall::scale_damage(CGameTrace& enter_trace, CCSWeapData* weapon_data, float& current_damage)
{
	if (!g_LocalPlayer || !g_LocalPlayer->is_alive())
		return;

	C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon();
	if (!weapon)
		return;

	auto targ_entity = static_cast<C_BasePlayer*>(enter_trace.hit_entity);
	const bool has_heavy_armor = targ_entity->m_bHasHeavyArmor(); //changed this from false to ->has_heavy_armor
	const int armor_value = targ_entity->m_ArmorValue();
	const int hit_group = enter_trace.hitgroup;

	if (weapon->is_taser() || weapon->is_grenade() || weapon->is_knife())
		return;

	const auto is_armored = [&]()-> bool
	{
		switch (enter_trace.hitgroup)
		{
		case HITGROUP_HEAD:
			return !!targ_entity->m_bHasHelmet();
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			return true;
		default:
			return false;
		}
	};

	switch (hit_group)
	{
	case HITGROUP_HEAD:
		current_damage *= 2.f;
		break;
	case HITGROUP_STOMACH:
		current_damage *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		current_damage *= 0.75f;
		break;
	default:
		break;
	}

	if (armor_value > 0 && is_armored())
	{
		float bonus_value = 1.f;
		float armor_bonus_ratio = 0.5f;
		float armor_ratio = weapon_data->armor_ratio / 2.f;

		if (has_heavy_armor)
		{
			armor_bonus_ratio = 0.33f;
			armor_ratio *= 0.5f;
			bonus_value = 0.33f;
		}

		auto new_damage = current_damage * armor_ratio;

		if (((current_damage - (current_damage * armor_ratio)) * (bonus_value * armor_bonus_ratio)) > armor_value)
		{
			new_damage = current_damage - (armor_value / armor_bonus_ratio);
		}

		current_damage = new_damage;
	}
}

bool c_autowall::trace_to_exit(CGameTrace& enter_trace, CGameTrace& exit_trace, Vector start_pos, Vector direction)
{
	float dist = 0.f;

	while (dist <= 90.f)
	{
		dist += 4.f;
		Vector end = start_pos + direction * dist;

		const int point_contents = g_EngineTrace->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);
		if (point_contents & MASK_SHOT_HULL && !(point_contents & CONTENTS_HITBOX))
			continue;

		Vector a1 = end - direction * 4.f;
		Ray_t r1;
		r1.Init(end, a1);
		g_EngineTrace->TraceRay(r1, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exit_trace);

		if (exit_trace.startsolid && exit_trace.surface.flags & SURF_HITBOX)
		{
			CTraceFilterSkipEntity filter(exit_trace.hit_entity);
			Ray_t r2;
			r2.Init(end, start_pos);
			g_EngineTrace->TraceRay(r2, MASK_SHOT_HULL, &filter, &exit_trace);

			if ((exit_trace.fraction < 1.f || exit_trace.allsolid) && !exit_trace.startsolid)
			{
				end = exit_trace.endpos;
				return true;
			}

			continue;
		}

		if (!exit_trace.DidHit() || exit_trace.startsolid)
		{
			if (enter_trace.hit_entity && (enter_trace.hit_entity != nullptr && enter_trace.hit_entity != g_EntityList->
				GetClientEntity(0)))
			{
				exit_trace = enter_trace;
				exit_trace.endpos = start_pos + direction;
				return true;
			}

			continue;
		}

		if (!exit_trace.DidHit() || exit_trace.startsolid)
		{
			if (enter_trace.hit_entity != nullptr && enter_trace.hit_entity->EntIndex() != 0 && this->breakable_entity(
				static_cast<C_BaseEntity*>(enter_trace.hit_entity)))
			{
				exit_trace = enter_trace;
				exit_trace.endpos = start_pos + direction;
				return true;
			}

			continue;
		}

		if (exit_trace.surface.flags >> 7 & SURF_LIGHT && !(enter_trace.surface.flags >> 7 & SURF_LIGHT))
			continue;

		if (exit_trace.plane.normal.Dot(direction) <= 1.f)
		{
			end = end - direction * (exit_trace.fraction * 4.f);
			return true;
		}
	}

	return false;
}

bool c_autowall::handle_bullet_penetration(CCSWeapData* wpn_data, FireBulletData& data)
{
	CGameTrace trace_exit;

	auto* enter_surface_data = g_PhysSurface->GetSurfaceData(data.enter_trace.surface.surfaceProps);

	const int enter_material = enter_surface_data->game.material;

	const bool is_solid_surf = (data.enter_trace.contents >> 3) & CONTENTS_SOLID;
	const bool is_light_surf = (data.enter_trace.surface.flags >> 7) & SURF_LIGHT;

	if (wpn_data->penetration <= 0.f || data.penetrate_count <= 0)
		return false;

	if (
		(!data.penetrate_count && !is_light_surf && !is_solid_surf && enter_material != CHAR_TEX_GLASS && enter_material
			!= CHAR_TEX_GRATE)
		|| !trace_to_exit(data.enter_trace, trace_exit, data.enter_trace.endpos, data.direction)
		&& !(g_EngineTrace->GetPointContents(data.enter_trace.endpos, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr) & (
			MASK_SHOT_HULL | CONTENTS_HITBOX))
	)
		return false;

	SurfaceData_t* exit_surface_data = g_PhysSurface->GetSurfaceData(trace_exit.surface.surfaceProps);
	const int exit_material = exit_surface_data->game.material;
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = (enter_surface_data->game.penetrationmodifier + exit_surface_data
	                                                                                      ->game.penetrationmodifier) /
		2;

	// stops autowall from shooting a material on cache with inconsistent penetration values.
	// DE_CACHE/DE_CACHE_TELA_03
	// CS_ITALY/CR_MISCWOOD2B
	// commented out cuz .find is retarded, fuck you
	//if (m_enter_material_name.find("TELA") != std::string::npos && m_exit_material_name.find("ITALY") != std::string::
		//npos)
		//return false;

	if (enter_material == CHAR_TEX_GLASS || enter_material == CHAR_TEX_GRATE)
	{
		combined_penetration_modifier = 3.f;
		final_damage_modifier = 0.05f;
	}
	else if (is_light_surf || is_solid_surf)
	{
		combined_penetration_modifier = 1.f;
		final_damage_modifier = 0.16f;
	}

	if (enter_material == exit_material)
	{
		if (exit_material == CHAR_TEX_CARDBOARD || exit_material == CHAR_TEX_WOOD)
			combined_penetration_modifier = 3.f;
		else if (exit_material == CHAR_TEX_PLASTIC)
			combined_penetration_modifier = 2.0f;
	}

	const float modifier = std::fmaxf(0.f, 1.f / combined_penetration_modifier);
	const float pen_len = (trace_exit.endpos - data.enter_trace.endpos).Length();
	const float lost_dmg = modifier * 3.f * std::fmaxf(0.f, 3.f / wpn_data->penetration * 1.25f) + data.current_damage *
		final_damage_modifier + pen_len * pen_len * modifier / 24.f;

	if (lost_dmg > data.current_damage)
		return false;

	if (lost_dmg > 0.f)
		data.current_damage -= lost_dmg;

	if (data.current_damage <= 0)
		return false;

	data.src = trace_exit.endpos;
	data.penetrate_count--;

	return true;
}

bool c_autowall::simulate_fire_bullet(C_BaseCombatWeapon* weapon, FireBulletData& data)
{
	C_BasePlayer* localplayer = g_LocalPlayer;
	auto* weapon_info = weapon->get_cs_wpn_data();

	data.penetrate_count = 4;
	data.trace_length = 0.0f;
	data.current_damage = static_cast<float>(weapon_info->damage);

	while (data.penetrate_count > 0 && data.current_damage >= 1.0f)
	{
		data.trace_length_remaining = weapon_info->range - data.trace_length;

		Vector end = data.src + data.direction * data.trace_length_remaining;

		trace_line(data.src, end, MASK_SHOT, localplayer, &data.enter_trace);
		clip_trace_to_players(data.src, end * 40.f, MASK_SHOT, &data.filter, &data.enter_trace);

		if (data.enter_trace.fraction == 1.0f)
			break;

		if (data.enter_trace.hitgroup <= HITGROUP_RIGHTLEG && data.enter_trace.hitgroup > HITGROUP_GENERIC)
		{
			data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
			data.current_damage *= powf(weapon_info->range_modifier, data.trace_length * 0.002f);

			auto* player = dynamic_cast<C_BasePlayer*>(data.enter_trace.hit_entity);

			if (player->m_iTeamNum() == localplayer->m_iTeamNum())
				return false;

			ScaleDamage_1(data.enter_trace.hitgroup, player, weapon_info->armor_ratio, data.current_damage);

			return true;
		}

		if (!handle_bullet_penetration(weapon_info, data))
			break;
	}

	return false;
}


float c_autowall::can_hit(Vector& point)
{
	C_BasePlayer* local = g_LocalPlayer;

	if (!local || !local->is_alive())
		return -1.f;

	auto local_weapon = local->m_hActiveWeapon();

	if (!local_weapon)
		return -1.f;

	FireBulletData data(local->get_eye_pos());
	data.filter = CTraceFilter();
	data.filter.pSkip = local;
	const Vector tmp = point - local->get_eye_pos();

	data.direction = tmp;
	data.direction.NormalizeInPlace();

	float current_damage = 0;
	if (simulate_fire_bullet(local_weapon, data))
		current_damage = data.current_damage;

	return current_damage;
}

float c_autowall::best_hit_point(C_BasePlayer* entity, int prioritized, float min_dmg, Vector& out)
{
	if (!entity->GetModel())
		return 0.f;

	auto studio_mdl = g_MdlInfo->GetStudiomodel(entity->GetModel());

	if (!studio_mdl)
		return 0.f;

	const auto hitbox_set = studio_mdl->GetHitboxSet(0);

	if (!hitbox_set.has_value())
		return 0.f;

	const auto hitset = hitbox_set.value();
	
	auto hitbox = hitset->GetHitbox(prioritized);

	if (!hitbox.has_value())
		return 0.f;
	
	if (!hitbox.value())
		return 0.f;

	const auto index = entity->EntIndex();
	const auto matrix = player_matrixes[index].matrix;

	const auto hitbox_val = hitbox.value();

	std::vector<Vector> scan_list;

	Vector max;
	Vector min;
	Math::VectorTransform(hitbox_val->bbmax, matrix[hitbox_val->bone], max);
	Math::VectorTransform(hitbox_val->bbmin, matrix[hitbox_val->bone], min);

	const auto center = (min + max) * 0.5f;

	const QAngle current_angles = Math::CalcAngle(center, g_LocalPlayer->get_eye_pos());

	Vector forward;
	Math::AngleVectors(current_angles, forward);

	const Vector right = forward.Cross(Vector(0, 0, 1));
	const Vector left = Vector(-right.x, -right.y, right.z);

	const Vector top = Vector(0, 0, 1);
	const Vector bot = Vector(0, 0, -1);

	const static auto point_scale = .8;
	const Vector right_pt = right * (hitbox_val->m_flRadius * point_scale) + center;
	const Vector left_pt = left * (hitbox_val->m_flRadius * point_scale) + center;
	const Vector top_pt = top * (hitbox_val->m_flRadius * point_scale) + center;
	const Vector bot_pt = bot * (hitbox_val->m_flRadius * point_scale) + center;

	switch (prioritized)
	{
	case HITBOX_HEAD:
	case HITBOX_NECK:
	case HITBOX_PELVIS:
		scan_list.push_back(top_pt);
		scan_list.push_back(right_pt);
		scan_list.push_back(left_pt);
		scan_list.push_back(bot_pt);
		break;
	default:
		scan_list.push_back(top_pt);
		scan_list.push_back(right_pt);
		scan_list.push_back(left_pt);
		break;
	}

	float higher_damage = 0.f;
	for (auto cur : scan_list)
	{
		if (!cur.IsValid())
			continue;

		const float current_damage = can_hit(cur);

		if (current_damage <= 0.f)
			continue;

		if (current_damage > higher_damage && current_damage > min_dmg)
		{
			higher_damage = current_damage;
			out = cur;
		}
	}
	return higher_damage;
}


Vector c_autowall::calculate_best_point(C_BasePlayer* entity, int prioritized, float min_dmg, bool only_prioritized)
{
	auto matrix = player_matrixes[entity->EntIndex()].matrix;
	studiohdr_t* hdr = g_MdlInfo->GetStudiomodel(entity->GetModel());
	mstudiohitboxset_t* set = hdr->GetHitboxSet(0).value();
	Vector best_vec = Vector(0, 0, 0);

	if (only_prioritized && best_hit_point(entity, prioritized, min_dmg, best_vec) >= min_dmg)
	{
		return best_vec;
	}

	const static std::array<int, 16> hitbox_loop = {
		HITBOX_HEAD,
		HITBOX_NECK,
		HITBOX_PELVIS,
		HITBOX_STOMACH,
		HITBOX_CHEST,
		HITBOX_UPPER_CHEST,
		HITBOX_LEFT_THIGH,
		HITBOX_RIGHT_THIGH,
		HITBOX_LEFT_FOOT,
		HITBOX_RIGHT_FOOT,
		HITBOX_LEFT_HAND,
		HITBOX_RIGHT_HAND,
		HITBOX_LEFT_UPPER_ARM,
		HITBOX_LEFT_FOREARM,
		HITBOX_RIGHT_UPPER_ARM,
		HITBOX_RIGHT_FOREARM
	};

	float highest_dmg = 0.f;
	Vector cur_vec;
	
	for (const auto hitbox : hitbox_loop)
	{
		//bool vis_part = false;
		const float cur_vec_dmg = best_hit_point(entity, hitbox, min_dmg, cur_vec);

		if (cur_vec_dmg <= 0)
			continue;

		//this stuff is kind of redundant, checks for vis parts but best_hit_point already does that
		//if (vis_part = entity->is_vector_visible(cur_vec); vis_part)
		//{
		//	if (cur_vec_dmg > highest_dmg)
		//	{
		//		highest_dmg = cur_vec_dmg;
		//		best_vec = cur_vec;
		//	}
		//	continue;
		//}

		if (cur_vec_dmg > highest_dmg)
		{
			highest_dmg = cur_vec_dmg;
			best_vec = cur_vec;
		}
	}
	return best_vec;
}