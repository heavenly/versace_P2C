#include "local_info.h"


namespace info_lp
{
	int current_target_index;
	bool shot_last_tick;
	int spec_list_size = 0;

	bool player_hurt_triggered;
	bool weapon_fire_triggered;
	bool bullet_impact_triggered;

	std::string username;
	std::string hwid;

	std::string last_function_call;
	
	std::vector<std::uint64_t> vs_users = {};
};