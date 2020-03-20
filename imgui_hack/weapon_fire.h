#pragma once
#include "valve_sdk/interfaces/IGameEventmanager.hpp"
#include "singleton.hpp"

class weapon_fire_event : public IGameEventListener2, public Singleton<weapon_fire_event>
{
public:

	void fire_game_event(IGameEvent *event);
	int  get_event_debug_id(void);

	void register_self();
	void unregister_self();
};