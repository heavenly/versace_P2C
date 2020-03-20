#include "weapon_fire.h"
#include "valve_sdk/csgostructs.hpp"
#include "resolver.h"
#include "local_info.h"

void weapon_fire_event::fire_game_event(IGameEvent *event)
{
	if (!g_LocalPlayer || !event)
		return;

	//localplayer is firing
	if (g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) == g_EngineClient->GetLocalPlayer())
	{
		C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(info_lp::current_target_index);

		if (!entity)
			return;

		resolver.on_weapon_fire(entity);
	}

}

int weapon_fire_event::get_event_debug_id(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void weapon_fire_event::register_self()
{
	g_GameEvents->AddListener(this, "weapon_fire", false);
}

void weapon_fire_event::unregister_self()
{
	g_GameEvents->RemoveListener(this);
}