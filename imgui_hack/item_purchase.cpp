#include "item_purchase.h"
#include "valve_sdk/csgostructs.hpp"
#include "options.hpp"
#include "notification.h"

void item_purchase_event::fire_game_event(IGameEvent* event)
{
	if (!g_LocalPlayer || !event)
		return;

	if (!g_vars.vis_misc_log_purchases)
		return;

	const auto buyer = event->GetInt("userid");
	std::string gun = event->GetString("weapon");

	auto player = C_BasePlayer::GetPlayerByIndex(g_EngineClient->GetPlayerForUserID(buyer));
	auto player_info = player->get_player_info();
	if (player && player->m_iTeamNum() != g_LocalPlayer->m_iTeamNum())
	{
		if (gun.find("weapon_") != std::string::npos)
			gun.erase(gun.find("weapon_"), 7);
		else if (gun.find("item_") != std::string::npos)
			gun.erase(gun.find("item_"), 5);
		
		const std::string msg = std::string(player_info.szName) + " bought " + gun;
		push_notifications::Push(msg);
		g_CVar->ConsolePrintf(msg.c_str());
	}
}

int item_purchase_event::get_event_debug_id()
{
	return EVENT_DEBUG_ID_INIT;
}

void item_purchase_event::register_self()
{
	g_GameEvents->AddListener(this, "item_purchase", false);
}

void item_purchase_event::unregister_self()
{
	g_GameEvents->RemoveListener(this);
}