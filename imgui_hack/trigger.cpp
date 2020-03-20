#include "trigger.h"
#include "options.hpp"
#include "helpers/math.hpp"
#include "backtrack.h"
#include <chrono>

c_trigger trigger = c_trigger();

void c_trigger::do_trigger(CUserCmd* cmd)
{

	if (!g_vars.triggerbot_enabled)
		return;

	if (g_vars.triggerbot_key > 0 && !GetAsyncKeyState(g_vars.triggerbot_key))
		return;

	if (!g_LocalPlayer)
		return;

	if (!g_LocalPlayer->is_alive())
		return;

	auto local_weapon = g_LocalPlayer->m_hActiveWeapon();

	if (!local_weapon)
		return;

	auto epoch_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	if (epoch_time - last_epoch < g_vars.triggerbot_delay)
		return;

	last_epoch = epoch_time;

	auto weapon_auto = local_weapon->get_cs_wpn_data()->full_auto;
	static bool flip = true;

	if (is_over_player(cmd) || (g_vars.triggerbot_backtrack && backtrack.is_over_player_backtrack_record(cmd)))
	{
		if (weapon_auto)
			cmd->buttons |= IN_ATTACK;
		else
		{
			flip ? cmd->buttons |= IN_ATTACK : cmd->buttons &= (~IN_ATTACK);
			flip = !flip;
		}
	}
}

bool c_trigger::is_over_player(CUserCmd* cmd)
{
	Vector trace_end;
	trace_t tr;

	Math::AngleVectors(cmd->viewangles, trace_end);

	Vector trace_start = g_LocalPlayer->get_eye_pos();
	trace_end = trace_start + (trace_end * 8192.0f);

	Ray_t ray;
	ray.Init(trace_start, trace_end);

	CTraceFilter trace_filter;
	trace_filter.pSkip = g_LocalPlayer;
	g_EngineTrace->TraceRay(ray, 0x46004003, &trace_filter, &tr);

	C_BasePlayer* player = (C_BasePlayer*)tr.hit_entity;

	if (!player)
		return false;

	if (player->GetClientClass()->m_ClassID != ClassId::ClassId_CCSPlayer)
		return false;

	if (!player->is_alive())
		return false;

	if (player->is_dormant() || player->m_bGunGameImmunity())
		return false;

	if (player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum() && !g_vars.triggerbot_targets[0])
		return false;

	if (player->m_iTeamNum() != g_LocalPlayer->m_iTeamNum() && !g_vars.triggerbot_targets[1])
		return false;

	if (!g_vars.triggerbot_hitboxes[tr.hitgroup])
		return false;

	return true;
}
