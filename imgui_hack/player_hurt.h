#pragma once
#include "valve_sdk/interfaces/IGameEventmanager.hpp"
#include "singleton.hpp"
#include <vector>
#include <string>

struct hitmarker_info
{
	float exp_time;
	std::string dmg;
};

struct event_info
{
	std::string msg;
	float exp_time;
};

class player_hurt_event : public IGameEventListener2, public Singleton<player_hurt_event>
{
public:

	void fire_game_event(IGameEvent *event);
	int  get_event_debug_id(void);

	void register_self();
	void unregister_self();

	void paint(void);

private:

	std::vector<hitmarker_info> v_hitmarker_info;
	std::vector<event_info> v_event_info;
};