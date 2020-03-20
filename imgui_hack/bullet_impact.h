#pragma once
#include "valve_sdk/interfaces/IGameEventmanager.hpp"
#include "singleton.hpp"
#include "valve_sdk/math/Vector.hpp"
#include "valve_sdk/csgostructs.hpp"
#include <vector>

struct bullet_impact_info
{
	float exp_time;
	Vector hit_pos;
	int player_type; //0 - local, 1 - team, 2 - enemy
	C_BasePlayer* player_who_shot;
};

class bullet_impact_event : public IGameEventListener2, public Singleton<bullet_impact_event>
{
public:

	void fire_game_event(IGameEvent *event);
	int  get_event_debug_id(void);

	void register_self();
	void unregister_self();

	void paint(void);

private:

	std::vector<bullet_impact_info> v_bullet_info;
};