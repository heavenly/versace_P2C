#include "player_hurt.h"
#include "valve_sdk/csgostructs.hpp"
#include "visuals_pt.h"
#include "options.hpp"
#include "kaching.h"
#include "resolver.h"
#include "notification.h"


void player_hurt_event::fire_game_event(IGameEvent *event)
{
	if (!g_LocalPlayer || !event)
		return;

	auto get_hitbox = [](int hitbox) -> std::string
	{
		switch (hitbox)
		{
		case 1: return "head";
		case 2: return "chest";
		case 3: return "stomach";
		case 4: return "left arm";
		case 5: return "right arm";
		case 6: return "left leg";
		case 7: return "right leg";
		default: return "null";
		}
	};

	if (g_EngineClient->GetPlayerForUserID(event->GetInt("attacker")) == g_EngineClient->GetLocalPlayer() && g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) != g_EngineClient->GetLocalPlayer())
	{
		auto entity = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt("userid")));
		int damage = event->GetInt("dmg_health");
		int health_left = event->GetInt("health");
		if (g_vars.vis_misc_hitmarkers)
		{
			//add bt tick
			std::string info = std::to_string(damage) + " dmg";

			if (g_vars.vis_misc_hitmarkers_bt_tick && g_vars.backtrack_enabled)
				info += " backtracked " + std::to_string(etc_player_info::backtrack_tick[entity->EntIndex()]) + " ticks";

			if (g_vars.vis_misc_hitmarkers_hitbox)
				info += " hitbox " + get_hitbox(event->GetInt("hitgroup"));

			v_hitmarker_info.push_back({ g_GlobalVars->curtime + 1, info });
			switch (g_vars.vis_misc_hitmarkers_style)
			{
			case 0:g_EngineClient->ExecuteClientCmd("play buttons\\arena_switch_press_02.wav"); break;
			case 1: PlaySound((LPCWSTR)sound_effect_wav, NULL, SND_MEMORY | SND_ASYNC); break;
			}
		}

		if (g_vars.vis_misc_hurt_notifications)
		{
			std::string hurt_info = "hit " + std::string(entity->get_player_info().szName) + " in " + get_hitbox(event->GetInt("hitgroup")) + " for " + std::to_string(damage) + " damage (" + std::to_string(health_left) + " hp left.)";
			push_notifications::Push(hurt_info, health_left <= 0 ? Color(0, 120, 0) : Color(120, 0, 0));
		}

		if(g_vars.esp_hurt_hitboxes)
		{
			visuals_pt::Player::draw_hitboxes(entity, g_vars.color_esp_hurt_hitboxes);
		}

		/*if (g_vars.esp_hurt_hitboxes)
		{
			auto player_by_index = [](int index) -> C_BasePlayer *
			{
				typedef C_BasePlayer* (__fastcall * PlayerByIndex)(int);
				static PlayerByIndex UTIL_PlayerByIndex = (PlayerByIndex)Utils::PatternScan(GetModuleHandleA("server.dll"), "85 C9 7E 2A A1");
				if (!UTIL_PlayerByIndex)
					return false;
				return UTIL_PlayerByIndex(index);
			};

			static uintptr_t call_to_draw_server = (uintptr_t)Utils::PatternScan(GetModuleHandleA("server.dll"), "55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE");
			float duration = g_vars.esp_hurt_hitboxes_time;
			PVOID ent = player_by_index(entity->EntIndex());
			if (ent)
			{
				__asm
				{
					pushad
					movss xmm1, duration
					push 0 //bool monoColor
					mov ecx, ent
					call call_to_draw_server
					popad
				}
			}
		}*/
		//^ only works in sv_cheats 1

		resolver.on_player_hurt(entity);
	}
}

int player_hurt_event::get_event_debug_id(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void player_hurt_event::register_self()
{
	g_GameEvents->AddListener(this, "player_hurt", false);
}

void player_hurt_event::unregister_self()
{
	g_GameEvents->RemoveListener(this);
}

void player_hurt_event::paint()
{
	static int width = 0, height = 0;

	if (width == 0 || height == 0)
		g_EngineClient->GetScreenSize(width, height);

	float alpha = 0.f;

	if (!g_vars.vis_misc_hitmarkers)
		return;

	for (size_t i = 0; i < v_hitmarker_info.size(); i++)
	{
		const float diff = v_hitmarker_info.at(i).exp_time - g_GlobalVars->curtime;

		if (diff <= 0.f || diff > 1.f)
		{
			v_hitmarker_info.erase(v_hitmarker_info.begin() + i);
			continue;
		}

		constexpr int dist = 24;

		const float ratio = 1.f - (diff / 1);
		alpha = 1 - diff / 1;

		const Color c = Color(g_vars.color_hitmarker_text.r(), g_vars.color_hitmarker_text.g(), g_vars.color_hitmarker_text.b(), (int)(alpha * 255.f));
		visuals_pt::draw_string(c, (v_hitmarker_info.at(i).dmg).c_str(), width / 2 + 6 + ratio * dist / 2, height / 2 + 6 + ratio * dist);
	}

	if (!v_hitmarker_info.empty())
	{
		const Color c = Color(g_vars.color_hitmarker.r(), g_vars.color_hitmarker.g(), g_vars.color_hitmarker.b(), (int)(alpha * 255.f));
		constexpr int line_size = 12;
		g_VGuiSurface->DrawSetColor(c);
		g_VGuiSurface->DrawLine(width / 2 - line_size / 2, height / 2 - line_size / 2, width / 2 + line_size / 2, height / 2 + line_size / 2);
		g_VGuiSurface->DrawLine(width / 2 + line_size / 2, height / 2 - line_size / 2, width / 2 - line_size / 2, height / 2 + line_size / 2);
	}
}