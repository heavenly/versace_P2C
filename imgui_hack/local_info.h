#pragma once
#include <iostream>
#include <vector>

namespace info_lp
{
	extern int current_target_index;
	extern bool shot_last_tick;
	extern int spec_list_size;

	extern bool player_hurt_triggered;
	extern bool weapon_fire_triggered;
	extern bool bullet_impact_triggered;

	extern std::string username;
	extern std::string hwid;

	extern std::string last_function_call;

	extern std::vector<std::uint64_t> vs_users;
};