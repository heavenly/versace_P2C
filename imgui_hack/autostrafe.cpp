#include "autostrafe.h"
#include "helpers/math.hpp"
#include "options.hpp"

void autostrafer::auto_jump(CUserCmd* cmd)
{
	if (!g_vars.misc_bhop)
		return;

	if (!g_LocalPlayer)
		return;

	if (!g_LocalPlayer->is_alive())
		return;

	auto flags = g_LocalPlayer->m_fFlags();

	static int cur_jumps = 0;
	int total_jumps = g_vars.misc_jumps_per_tick > 1 ? g_vars.misc_jumps_per_tick + rand() % 6 + 3 : g_vars.misc_jumps_per_tick;

	if (cmd->buttons & IN_JUMP)
	{
		if (!(flags & FL_ONGROUND) && rand() % 100 <= g_vars.misc_bhop_chance)
		{
				cmd->buttons &= ~IN_JUMP;
		}
	}

	if (flags & FL_ONGROUND)
	{
		cur_jumps = 0;
	}

	//testing
	if (!(flags & FL_ONGROUND) && cur_jumps < total_jumps && g_vars.misc_jumps_per_tick > 1)
	{
		cmd->tick_count % 2 ? cmd->buttons &= ~IN_JUMP : cmd->buttons |= IN_JUMP;
		cur_jumps++;
	}
}

void autostrafer::do_strafes(CUserCmd * cmd)
{
	if (!g_vars.misc_autostrafe)
		return;

	if (!g_LocalPlayer)
		return;

	if (!g_LocalPlayer->is_alive())
		return;

	//dont strafe if player is not in air
	if (g_LocalPlayer->m_fFlags() & FL_ONGROUND || g_LocalPlayer->m_MoveType() & MOVETYPE_LADDER || g_LocalPlayer->m_MoveType() & MOVETYPE_NOCLIP)
		return;

	//actual autostrafer(s) (not great, pretty common)
	//works pretty well actually
	//setting buttons is a shitty bypass meme
	if (g_vars.misc_autostrafe_type == 0)
	{
		if (cmd->mousedx > 1)
		{
			cmd->sidemove = 450;
			cmd->buttons |= IN_MOVERIGHT;
		}
		else if (cmd->mousedx < -1)
		{
			cmd->sidemove = -450;
			cmd->buttons |= IN_MOVELEFT;
		}
	}
	else if (g_vars.misc_autostrafe_type == 1)
	{
		if (cmd->mousedx > 1)
		{
			cmd->sidemove = -450;
			cmd->buttons |= IN_MOVELEFT;
		}
		else if (cmd->mousedx < -1)
		{
			cmd->sidemove = 450;
			cmd->buttons |= IN_MOVERIGHT;
		}
	}
	else if (g_vars.misc_autostrafe_type == 2)
	{
		auto random_number = rand() % 100;
		if (random_number >= g_vars.misc_autostrafe_optimizer_chance)
		{
			if (cmd->mousedx > 1)
			{
				cmd->sidemove = 450;
				cmd->buttons |= IN_MOVERIGHT;
			}
			else if (cmd->mousedx < -1)
			{
				cmd->sidemove = -450;
				cmd->buttons |= IN_MOVELEFT;
			}
		}
	}
	else if (g_vars.misc_autostrafe_type == 3)
	{
		QAngle viewangles;
		g_EngineClient->GetViewAngles(&viewangles);

		static bool side_switch = false;
		side_switch = !side_switch;

		cmd->forwardmove = 0.f;
		cmd->sidemove = side_switch ? 450.f : -450.f;

		float velocity_yaw = g_LocalPlayer->m_vecVelocity().y;

		auto clamp = [=](float f1, float min, float max) -> float
		{
			if (f1 < min)
				return min;

			if (f1 > max)
				return max;

			return f1;
		};

		float rotation = clamp(RAD2DEG(std::atan2(15.f, g_LocalPlayer->m_vecVelocity().Length2D())), 0.f, 90.f);

		auto normalize_yaw = [](float yaw) -> float
		{
			if (yaw < -180)
				return -180;
			else if (yaw > 180)
				return 180;

			return yaw;
		};

		float abs_yaw = std::fabs(normalize_yaw(velocity_yaw - viewangles.yaw));

		float ideal_yaw_rotation = (side_switch ? rotation : -rotation) + (abs_yaw < 5.f ? velocity_yaw : viewangles.yaw);

		float cos_rot = std::cos(DEG2RAD(viewangles.yaw - ideal_yaw_rotation));
		float sin_rot = std::sin(DEG2RAD(viewangles.yaw - ideal_yaw_rotation));

		cmd->forwardmove = (cos_rot * cmd->forwardmove) - (sin_rot * cmd->sidemove);
		cmd->sidemove = (sin_rot * cmd->forwardmove) + (cos_rot * cmd->sidemove);
	}
}

void autostrafer::edge_jump(CUserCmd * cmd)
{
	if (!g_vars.misc_edge_jump)
		return;

	if (g_vars.misc_edge_jump_key > 0 && !GetAsyncKeyState(g_vars.misc_edge_jump_key))
		return;

	if (cmd->buttons & IN_DUCK && g_vars.misc_edge_jump_crouch && !(g_LocalPlayer->m_fFlags() & FL_ONGROUND))
	{
		cmd->buttons &= ~IN_DUCK;
	}

	Vector forward_vec;
	Math::AngleVectors(cmd->viewangles, forward_vec);
	Vector end_vec = g_LocalPlayer->get_eye_pos() + (forward_vec * 5);
	Vector final_vec = end_vec;
	final_vec.z -= (g_LocalPlayer->m_vecViewOffset().z + 15);

	//ok cool
	auto get_visible = [&](Vector& abs_start, Vector& abs_end, C_BasePlayer* p_entity) -> bool
	{
		trace_t tr;
		Ray_t ray;
		CTraceFilter filter;
		filter.pSkip = g_LocalPlayer;

		ray.Init(abs_start, abs_end);
		g_EngineTrace->TraceRay(ray, MASK_NPCWORLDSTATIC | CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_HITBOX, &filter, &tr);

		return (tr.hit_entity == p_entity || tr.fraction > 0.99f);
	};

	if (get_visible(end_vec, final_vec, g_LocalPlayer)) 
	{
		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
		{
			cmd->buttons |= IN_JUMP;
			if (g_vars.misc_edge_jump_crouch)
			{
				cmd->buttons |= IN_DUCK;
			}
		}
	}
}
