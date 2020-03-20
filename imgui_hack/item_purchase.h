#pragma once
#include "valve_sdk/interfaces/IGameEventmanager.hpp"
#include "singleton.hpp"

class item_purchase_event : public IGameEventListener2, public Singleton<item_purchase_event>
{
public:

	void fire_game_event(IGameEvent* event);
	int  get_event_debug_id();

	void register_self();
	void unregister_self();
};