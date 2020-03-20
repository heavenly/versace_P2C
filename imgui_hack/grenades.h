#pragma once
#include "valve_sdk/interfaces/IGameEventmanager.hpp"
#include "singleton.hpp"
#include "valve_sdk/math/Vector.hpp"
#include <vector>

struct grenade
{
	int type;
	Vector location;
};

class grenade_detonate_event : public IGameEventListener2, public Singleton<grenade_detonate_event>
{
public:

	void fire_game_event(IGameEvent *event);
	int  get_event_debug_id(void);

	void register_self();
	void unregister_self();

	void paint();

private:

	std::vector<grenade> v_grenade_info;
};