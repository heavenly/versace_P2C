#pragma once
#include "valve_sdk/csgostructs.hpp"
#include <array>

struct resolver_player_info
{
	float moving_lby;
	int shots_missed = 0;
};

class c_resolver
{
public:
	std::array<resolver_player_info, 65> player_res_info;
	void log_player_info();

	void on_player_hurt(C_BasePlayer* entity);
	void on_weapon_fire(C_BasePlayer* entity);
	void on_bullet_impact(Vector impact_pos);
	void do_resolver();
private:
	float get_standing_yaw(C_BasePlayer* player);
	float get_moving_yaw(C_BasePlayer* player);
	Vector get_intersection_point(const Vector& start, const Vector& end, const Vector& mins, const Vector& maxs, float radius);
};

extern c_resolver resolver;