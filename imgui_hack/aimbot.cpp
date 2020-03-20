#include "aimbot.h"
#include "local_info.h"
#include "autowall.h"
#include "anticheat.h"
#include "trigger.h"
#include "helpers/math.hpp"
#include "helpers/utils.hpp"
#include "options.hpp"

int time_to_ticks(float dt)
{
	return (int)(0.5f + (float)(dt) / g_GlobalVars->interval_per_tick);
}

float ticks_to_time(int t)
{
	return g_GlobalVars->interval_per_tick * (t);
}

aimbot_data c_aimbot::get_best_entity(CUserCmd* cmd)
{
	C_BasePlayer* best_entity = nullptr;
	float best_fov = FLT_MAX;
	for (size_t i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{

		auto* entity = dynamic_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
		if (!entity)
			continue;

		if (!entity->is_alive())
			continue;

		if (entity->is_dormant() || entity->m_bGunGameImmunity())
			continue;

		if ((entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum() && !g_vars.aimbot_targets[0]) ||
			entity->m_iTeamNum() == 0 ||
			(entity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum() && !g_vars.aimbot_targets[1]))
			continue;

		if (g_vars.aimbot_get_best_hitbox)
			_best_hitbox = get_best_hitbox(entity, cmd);

		if (g_vars.aimbot_get_best_hitbox && _best_hitbox == Hitboxes::HITBOX_MAX)
			continue;

		if (!g_vars.aimbot_get_best_hitbox)
		{
			if (g_vars.aimbot_vis_check && !entity->is_vector_visible(entity->get_hitbox_pos(g_vars.aimbot_hitbox)))
				continue;
		}

		if (!g_vars.hitscan_enabled && g_vars.aimbot_autowall && c_autowall::Get().can_hit(entity->get_hitbox_pos(g_vars.aimbot_hitbox)) < g_vars.aimbot_autowall_dmg)
			continue;

		if (g_vars.hitscan_enabled)
			_best_aim_pos = c_autowall::Get().calculate_best_point(entity, g_vars.aimbot_hitbox, g_vars.aimbot_autowall_dmg, false);

		if (g_vars.hitscan_enabled && _best_aim_pos == Vector(0, 0, 0))
			continue;

		const QAngle final_angle = Math::CalcAngle(g_LocalPlayer->get_eye_pos(), entity->get_hitbox_pos(g_vars.aimbot_hitbox));
		const float FOV = Math::GetFOV(cmd->viewangles, final_angle);

		if (FOV > best_fov || FOV > g_vars.aimbot_fov)
			continue;

		best_fov = FOV;
		best_entity = entity;
	}

	return { best_entity, best_fov };
}

int c_aimbot::get_best_hitbox(C_BasePlayer* entity, CUserCmd* cmd)
{
	if (!entity || !entity->is_alive() || entity->is_dormant() || entity->m_bGunGameImmunity())
		return Hitboxes::HITBOX_MAX;

	if (Utils::get_epoch_time() - _last_epoch_hitbox_scan[entity->EntIndex()] < g_vars.aimbot_get_best_hitbox_scan_time)
		return Hitboxes::HITBOX_MAX;

	float best_fov = FLT_MAX;
	int best_hitbox = Hitboxes::HITBOX_MAX;

	for (int hitbox = 0; hitbox < Hitboxes::HITBOX_MAX; hitbox++)
	{
		if (hitbox == Hitboxes::HITBOX_MAX)
			continue;

		auto hitbox_pos = entity->get_hitbox_pos(hitbox);

		if (g_vars.aimbot_vis_check && !entity->is_vector_visible(hitbox_pos))
			continue;

		if (g_vars.aimbot_smoke_check && line_goes_through_smoke(g_LocalPlayer->get_eye_pos(), hitbox_pos))
			continue;

		const float fov = Math::GetFOV(cmd->viewangles, Math::CalcAngle(g_LocalPlayer->get_eye_pos(), hitbox_pos));

		if (fov < g_vars.aimbot_fov && fov < best_fov)
		{
			best_hitbox = hitbox;
			best_fov = fov;
		}
	}

	_last_epoch_hitbox_scan[entity->EntIndex()] = Utils::get_epoch_time();
	return best_hitbox;
}

bool c_aimbot::line_goes_through_smoke(Vector start_pos, Vector end_pos)
{
	return Utils::line_goes_through_smoke(start_pos, end_pos);
}

void c_aimbot::do_aimbot(CUserCmd * cmd)
{

	if (!g_vars.aimbot_enabled)
		return;

	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!cmd)
		return;

	if (!g_LocalPlayer)
		return;

	if (!g_LocalPlayer->is_alive())
		return;

	if (g_vars.aimbot_on_key > 0 && !GetAsyncKeyState(g_vars.aimbot_on_key))
		return;

	if (g_vars.aimbot_aim_chance && rand() % 100 > g_vars.aimbot_aim_chance_percent)
		return;

	const auto epoch_time = Utils::get_epoch_time();

	if (epoch_time - _last_epoch < g_vars.aimbot_target_time && g_vars.aimbot_target_cooldown)
		return;

	C_BaseCombatWeapon* local_weapon = g_LocalPlayer->m_hActiveWeapon();

	if (!local_weapon)
		return;

	if (local_weapon->is_defuse_kit() || local_weapon->is_grenade() || local_weapon->is_knife())
		return;

	if (local_weapon->m_iClip1() <= 0)
		return;

	auto* weapon_info = local_weapon->get_cs_wpn_data();

	if (!weapon_info)
		return;

	if (g_LocalPlayer->m_iShotsFired() < g_vars.aimbot_aim_after_shots)
		return;

	if (g_vars.aimbot_speed_limit && g_vars.aimbot_speed < g_LocalPlayer->m_vecVelocity().Length2D())
		return;

	const auto gbe = get_best_entity(cmd);
	auto* target_entity = gbe.best_entity;
	if (g_vars.aimbot_get_best_hitbox)
		_best_hitbox = get_best_hitbox(target_entity, cmd);

	if (!target_entity)
		return;

	if (!target_entity->is_alive())
		return;

	if (g_vars.aimbot_get_best_hitbox && _best_hitbox == Hitboxes::HITBOX_MAX)
		return;

	if (target_entity != _last_target)
	{
		_last_target = target_entity;
		_last_epoch = epoch_time;
	}

	info_lp::current_target_index = target_entity->EntIndex();

	if (!(target_entity->m_fFlags() & FL_ONGROUND) && g_vars.aimbot_avoid_jumping)
		return;

	if (g_vars.aimbot_on_attack && !(cmd->buttons & IN_ATTACK))
		return;

	if (g_vars.aimbot_flash_check && g_LocalPlayer->m_flFlashDuration() > 0)
		return;

	if (g_vars.aimbot_non_sticky && trigger.is_over_player(cmd))
		return;

	auto enemy_hitbox_pos = target_entity->get_hitbox_pos(g_vars.aimbot_hitbox);
	if (g_vars.hitscan_enabled)
		enemy_hitbox_pos = _best_aim_pos;
	else if (g_vars.aimbot_get_best_hitbox)
		enemy_hitbox_pos = target_entity->get_hitbox_pos(_best_hitbox);

	QAngle final_angle = Math::CalcAngle(g_LocalPlayer->get_eye_pos(), enemy_hitbox_pos);

	if (g_vars.misc_rcs)
		final_angle -= g_LocalPlayer->m_aimPunchAngle() * g_vars.misc_rcs_amount;

	Math::Clamp(final_angle);

	if (final_angle.IsZero())
		return;

	if (g_vars.aimbot_smooth <= 0)
	{
		cmd->viewangles = final_angle;
		if (g_vars.aimbot_autoscope_enabled && local_weapon->is_sniper() && !g_LocalPlayer->m_bIsScoped())
			cmd->buttons |= IN_ATTACK2;
		else if (g_vars.aimbot_autoshoot_enabled)
		{
			const auto is_weapon_auto = local_weapon->get_cs_wpn_data()->full_auto;
			static bool flip = true;
			if (is_weapon_auto)
				cmd->buttons |= IN_ATTACK;
			else
			{
				flip ? cmd->buttons |= IN_ATTACK : cmd->buttons &= (~IN_ATTACK);
				flip = !flip;
			}
			
			const auto best_tick = time_to_ticks(anticheat.best_time[target_entity->EntIndex()]);
			if (cmd->tick_count - best_tick < time_to_ticks(.2))
				cmd->tick_count = best_tick;
		}
	}
	else
	{
		QAngle delta = final_angle - cmd->viewangles;
		Math::Clamp(delta);
		float smooth = powf(g_vars.aimbot_smooth, 0.4f);
		smooth = std::min(0.99f, smooth);
		QAngle to_change = delta - delta * smooth;
		Math::Clamp(to_change);
		//setting and clamping
		cmd->viewangles += to_change;
	}
	Math::Clamp(cmd->viewangles);

	if (g_vars.aimbot_autostop)
	{
		Vector velocity = g_LocalPlayer->m_vecVelocity();
		Vector direction = velocity.Angle();
		const float speed = velocity.Length();

		direction.y = cmd->viewangles.yaw - direction.y;

		const Vector negated_direction = direction * -speed;

		if (velocity.Length() > 0)
		{
			cmd->forwardmove = negated_direction.x;
			cmd->sidemove = negated_direction.y;
		}
	}

	if (!g_vars.aimbot_silent_aim)
		g_EngineClient->SetViewAngles(&cmd->viewangles);
}