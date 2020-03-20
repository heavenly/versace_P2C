#pragma once
#include "valve_sdk/interfaces/IGameEventmanager.hpp"
#include "singleton.hpp"

class call_vote_event : public IGameEventListener2, public Singleton<call_vote_event>
{
public:

	void fire_game_event(IGameEvent *event);
	int  get_event_debug_id(void);

	void register_self();
	void unregister_self();

private:
	int potential_votes = 0;
};