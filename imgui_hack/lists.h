#pragma once
#include <iostream>
#include <vector>

namespace menu_lists
{
	using v_string = std::vector<std::string>;
	extern v_string hitboxes;
	extern v_string skyboxes;
	extern v_string backtrack_chams_styles;
	extern v_string glow_styles;
	extern v_string autostrafe_types;
	extern v_string resolver_type;
	extern v_string hitboxes_shortened;
	extern v_string origin_box_styles;
	extern v_string distance_styles;
	extern std::vector<std::pair<std::string, int>> weapon_list;
	extern v_string weapon_list_names;
	extern v_string knife_names;
	extern std::vector<std::pair<std::string, int>> knife_list;
	extern v_string hitmarker_sounds;
	extern v_string resolve_base_angles;
	extern v_string bullet_beam_types;
	extern v_string execution_loc_types;
};