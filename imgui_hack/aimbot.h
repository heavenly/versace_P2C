#pragma once
#include "valve_sdk/csgostructs.hpp"
#include <array>

struct aimbot_data
{
	C_BasePlayer* best_entity;
	float best_fov;
};

class c_aimbot
{
public:
	void do_aimbot(CUserCmd* cmd);
private:
	bool line_goes_through_smoke(Vector start_pos, Vector end_pos);
	aimbot_data get_best_entity(CUserCmd* cmd);
	int get_best_hitbox(C_BasePlayer* entity, CUserCmd* cmd);

	long long _last_epoch;
	std::array<long long, 65> _last_epoch_hitbox_scan;
	C_BasePlayer* _last_target;
	Vector _best_aim_pos = Vector(0, 0, 0);
	int _best_hitbox = Hitboxes::HITBOX_MAX;

	//bool _was_visible;
	//long long _last_visible_time;
};
