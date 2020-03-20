#include "misc_features.h"
#include "options.hpp"
#include <array>
#include "local_info.h"
#include "helpers/math.hpp"

void c_misc_features::do_quickswitch(CUserCmd * cmd)
{
	if (!g_vars.misc_quickswitch)
		return;

	if (info_lp::shot_last_tick && !(cmd->buttons & IN_ATTACK))
	{
		info_lp::shot_last_tick = false;
		//ghetto way
		g_EngineClient->ExecuteClientCmd("slot3");
	}

	if (cmd->buttons & IN_ATTACK)
	{
		info_lp::shot_last_tick = true;
	}
}

void c_misc_features::log_quickswitch(CUserCmd * cmd)
{
	if (!g_LocalPlayer)
		return;

	if (!g_LocalPlayer->is_alive())
		return;

	auto local_wep = g_LocalPlayer->m_hActiveWeapon();

	if (!local_wep)
		return;

	if (cmd->buttons & IN_ATTACK && local_wep->m_flNextPrimaryAttack() < g_GlobalVars->curtime)
	{
		info_lp::shot_last_tick = true;
	}
}

void c_misc_features::infinite_crouch(CUserCmd * cmd)
{
	if (!g_vars.misc_fast_duck)
		return;

	cmd->buttons |= IN_BULLRUSH;
}

void c_misc_features::change_clantag()
{
	if (!g_vars.misc_clantag_changer)
		return;

	//if (g_ClientState->m_choked_commands > 0) //broken
		//return;
	
	//static std::string cur_clantag = "VER$ACE     ";
	//shitty autistic way, use std::rotate next time

	const static std::array<std::string, 13> clantags = { "VER$ACE", "VER$AC", "VER$A", "VER$", "VER", "VE", "V", "VE", "VER", "VER$", "VER$A", "VER$AC", "VER$ACE" };
	Utils::SetClantag(clantags[(g_GlobalVars->tickcount / 50) % clantags.size()].c_str());
}

void c_misc_features::restrict_speed(CUserCmd * cmd)
{
	if (!g_vars.misc_silent_walk || cmd->buttons & IN_WALK)
		return;

	auto weapon_handle = g_LocalPlayer->m_hActiveWeapon();

	if (!weapon_handle)
		return;

	auto velocity = g_LocalPlayer->m_vecVelocity();

	float speed = velocity.Length2D();

	if (speed >= (weapon_handle->get_cs_wpn_data()->max_speed / 3))
	{
		cmd->forwardmove = -cmd->forwardmove;
		cmd->sidemove = -cmd->sidemove;
	}
}

void c_misc_features::auto_derank()
{
	if (!g_vars.misc_auto_derank)
		return;

	static long long last_disconnect_time = Utils::get_epoch_time();
	static long long last_connected_time = Utils::get_epoch_time();

	if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected())
	{
		if (Utils::get_epoch_time() - last_connected_time > 7500)
		{
			g_EngineClient->ExecuteClientCmd("disconnect");
			last_disconnect_time = Utils::get_epoch_time();
		}
	}

	if (!g_EngineClient->IsInGame())
	{
		if (Utils::get_epoch_time() - last_disconnect_time > 15000) //15s
		{
			g_EngineClient->ExecuteClientCmd("retry");
			last_connected_time = Utils::get_epoch_time();
		}
	}
}

void c_misc_features::set_fakelag_limit()
{
	static auto choke_clamp = Utils::PatternScan(GetModuleHandleA("engine.dll"), "B8 ? ? ? ? 3B F0 0F 4F F0 89 5D FC").value() + 1;

	unsigned long protect = 0;

	constexpr auto size_of_int = sizeof(int);
	VirtualProtect((void*)choke_clamp, size_of_int, PAGE_EXECUTE_READWRITE, &protect);
	*(std::uint32_t*)choke_clamp = 62; //https://www.unknowncheats.me/forum/counterstrike-global-offensive/307478-real-fakelag.html
	VirtualProtect((void*)choke_clamp, size_of_int, protect, &protect);
}

void c_misc_features::crash_server()
{
	//broken
	/*auto nc = uintptr_t(g_EngineClient->GetNetChannelInfo());

	(*(void(__thiscall * *)(uintptr_t, float))(nc + 124))(nc, 3600.0); //set_timeout

	if (GetAsyncKeyState(0x78) & 1)
	{
		for (int i = 0; i < 4000; i++)
		{
			//netchannel->request_file or some shit
			(*(void(__thiscall * *)(uintptr_t, char*))(nc + 248))(nc, ".txt"); //linux crasher
			(*(void(__thiscall * *)(uintptr_t, char*))(nc + 248))(nc, "\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a.txt");
		}
	}*/	
}

void c_misc_features::copy_movement_bot(CUserCmd* cmd)
{
	if (!g_vars.misc_copy_bot)
		return;
	
	for (size_t i = 0; i < g_EngineClient->GetMaxClients(); i++) {
		C_BasePlayer* player = C_BasePlayer::GetPlayerByIndex(i);

		if (!player)
			continue;

		if (!player->is_alive() || player->is_dormant() || player == g_LocalPlayer)
			continue;

		if (cmd->buttons & IN_FORWARD || cmd->buttons & IN_BACK || cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_MOVERIGHT)
			return;

		int head_height = player->get_base_player_animstate()->m_flDuckAmount > 0 ? 46 : 64;
		auto is_directly_above = g_LocalPlayer->m_vecOrigin().DistTo(Vector(player->m_vecOrigin().x, player->m_vecOrigin().y, player->m_vecOrigin().z + head_height)) < 32.f;
		auto is_above = g_LocalPlayer->m_vecOrigin().z > (player->m_vecOrigin().z + head_height);
		if (is_above && is_directly_above) {
			// (c) https://www.unknowncheats.me/forum/2420084-post390.html
			auto velocity = player->m_vecVelocity();
			auto speed = velocity.Length();
			auto direction = velocity.Angle();

			direction.y = cmd->viewangles.yaw - direction.y;
			Vector forward;

			QAngle q_dir = QAngle(direction.x, direction.y, direction.z);
			Math::AngleVectors(q_dir, forward);
			
			cmd->forwardmove = forward.x * speed;
			cmd->sidemove = forward.y * speed;
		}
	}
}
