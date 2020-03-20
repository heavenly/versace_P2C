#pragma once
#include "valve_sdk/Misc/Color.hpp"
#include <array>

#define OPTION(type, var, val) type var = val

class Config
{
public:
	//
	// AIMBOT
	//
	bool aimbot_enabled = {};
	std::array<bool, 2> aimbot_targets = {};
	bool aimbot_silent_aim = {};
	bool aimbot_vis_check = {};
	bool aimbot_autowall = {};
	float aimbot_autowall_dmg = {};
	bool aimbot_autoshoot_enabled = {};
	bool aimbot_autoscope_enabled = {};
	bool aimbot_on_attack = {};
	int aimbot_aim_after_shots = {};
	bool aimbot_aim_chance = {};
	int aimbot_aim_chance_percent = {};
	int aimbot_on_key = {};
	bool aimbot_avoid_jumping = {};
	bool aimbot_smoke_check = {};
	bool aimbot_target_cooldown = {};
	float aimbot_target_time = {};
	bool aimbot_flash_check = {};
	float aimbot_fov = {};
	float aimbot_smooth = {};
	int aimbot_hitbox = {};
	bool aimbot_speed_limit = {};
	int aimbot_speed = {};
	bool hitscan_enabled = {};
	bool aimbot_get_best_hitbox = false;
	int aimbot_get_best_hitbox_scan_time = {};
	bool aimbot_get_best_backtrack = false;
	bool aimbot_non_sticky = false;
	bool aimbot_autostop = false;
	OPTION(bool, aimbot_reaction, false);
	OPTION(int, aimbot_reaction_time, 0);

	//triggerbot
	OPTION(bool, triggerbot_enabled, false);
	OPTION(bool, triggerbot_backtrack, false);
	std::array<bool, 2> triggerbot_targets = { false };
	std::array<bool, 11> triggerbot_hitboxes = { true, true, true, true , true , true , true , true , true , true , true };
	OPTION(int, triggerbot_key, 0);
	OPTION(int, triggerbot_delay, 0);
	OPTION(float, triggerbot_hitchance, 30);

	// 
	// ESP
	// 
	OPTION(bool, esp_enabled, false);
	OPTION(int, esp_on_key, 0);
	std::array<bool, 3> esp_targets = { false };
	OPTION(bool, esp_player_boxes, false);
	OPTION(int, esp_player_boxes_type, 0);
	OPTION(bool, esp_player_names, false);
	OPTION(bool, esp_player_health, false);
	OPTION(int, esp_player_health_thick, 3);
	OPTION(bool, esp_player_health_fractional, false);
	OPTION(bool, esp_player_armour, false);
	OPTION(int, esp_player_armour_thick, 3);
	OPTION(bool, esp_player_armour_fractional, false);
	OPTION(bool, esp_player_weapons, false);
	OPTION(bool, esp_player_snaplines, false);
	OPTION(bool, esp_crosshair, false);
	OPTION(bool, esp_planted_c4, false);
	OPTION(bool, esp_player_vulnerabilities, false);
	OPTION(bool, esp_aimbot_target, false);
	OPTION(bool, esp_hurt_hitboxes, false);
	OPTION(float, esp_hurt_hitboxes_time, 1);
	OPTION(bool, esp_farther, false);
	OPTION(bool, esp_offscreen, false);
	OPTION(int, esp_offscreen_width, 3);
	OPTION(int, esp_offscreen_height, 5);
	OPTION(bool, esp_origin_box, false);
	OPTION(int, esp_origin_box_style, 0);
	OPTION(float, esp_origin_box_size, 7);
	OPTION(bool, esp_player_money, false);
	OPTION(bool, esp_player_viewangles, false);
	OPTION(int, esp_viewangles_length, 400);
	OPTION(bool, esp_distance, false);
	OPTION(int, esp_distance_style, 0);
	OPTION(bool, esp_player_ammo_bar, false);
	OPTION(bool, esp_player_skeleton, false);
	OPTION(bool, esp_player_defuser, false);
	OPTION(bool, esp_player_c4, false);
	OPTION(bool, esp_player_planting_bar, false);
	OPTION(bool, esp_player_defusing_bar, false);
	OPTION(bool, esp_draw_vs_user, false);
	OPTION(bool, esp_draw_dangerzone_items, false);
	OPTION(int, esp_drawing_dist, 10000);


	OPTION(bool, glow_enabled, false);
	OPTION(int, glow_on_key, 0);
	OPTION(bool, glow_players, false);
	OPTION(bool, glow_others, false);
	OPTION(int, glow_others_style, 0);

	OPTION(bool, chams_enabled, false);
	std::array<bool, 3> chams_targets = { false };
	OPTION(bool, chams_ignorez, false);
	OPTION(bool, chams_wireframe, false);
	OPTION(bool, chams_flat, false);
	OPTION(bool, chams_glass, false);
	OPTION(bool, chams_reflective, false);
	OPTION(bool, chams_pulse, false);

	OPTION(bool, vis_misc_footstep_beams, false);
	OPTION(bool, vis_misc_bullet_beams, false);
	OPTION(int, vis_misc_bullet_beams_type, 0);
	OPTION(float, vis_misc_bullet_beams_size, 1.5);
	OPTION(float, vis_misc_bullet_beams_lifetime, 1);
	std::array<bool, 3> vis_misc_bullet_beams_targets = { false };
	OPTION(bool, vis_misc_bullet_impacts, false);
	OPTION(int, vis_misc_bullet_impacts_size, 1.5);
	OPTION(float, vis_misc_bullet_impacts_lifetime, 1);
	std::array<bool, 3> vis_misc_bullet_impacts_targets = {false};
	OPTION(int, vis_misc_skybox, 0);
	OPTION(bool, vis_misc_hitmarkers, false);
	OPTION(int, vis_misc_hitmarkers_style, 0);
	OPTION(bool, vis_misc_hitmarkers_bt_tick, false);
	OPTION(bool, vis_misc_hitmarkers_hitbox, false);
	OPTION(bool, vis_misc_grenade_circle, false);
	OPTION(bool, vis_misc_no_smoke, false);
	OPTION(bool, vis_misc_spread_crosshair, false);
	OPTION(bool, vis_misc_taser_range, false);
	OPTION(int, vis_misc_override_fov, 90);
	OPTION(int, vis_misc_override_viewmodel_fov, 1);
	OPTION(bool, vis_misc_draw_grenade_path, false);
	OPTION(bool, vis_misc_hud_features, false);
	OPTION(bool, vis_misc_hud_features_box, false);
	OPTION(bool, vis_misc_fov_ring, false);
	OPTION(bool, vis_misc_autowall_crosshair, false);
	OPTION(bool, vis_misc_hurt_notifications, false);
	OPTION(bool, vis_misc_log_purchases, false);
	

	//
	// MISC
	//
	OPTION(bool, misc_bhop, false);
	OPTION(int, misc_bhop_chance, 100);
	OPTION(int, misc_jumps_per_tick, 11);
	OPTION(bool, misc_autostrafe, false);
	OPTION(int, misc_autostrafe_type, 0);
	OPTION(int, misc_autostrafe_optimizer_chance, 100);
	OPTION(bool, misc_edge_jump, false);
	OPTION(bool, misc_edge_jump_crouch, false);
	OPTION(int, misc_edge_jump_key, 0);
	OPTION(bool, misc_legit_aa, false);
	OPTION(bool, misc_legit_aa_fakelag, false);
	OPTION(int, misc_legit_aa_fakelag_amount, 10);
	OPTION(bool, misc_legit_aa_slide_fix, false);
	OPTION(int, misc_legit_aa_ang, 36);
	OPTION(bool, misc_legit_aa_break_lby, false);
	OPTION(int, misc_legit_aa_break_lby_dist, 135);
	OPTION(bool, misc_legit_aa_pitch, false);
	OPTION(int, misc_legit_aa_pitch_val, 89);
	OPTION(bool, misc_clantag_changer, false);
	OPTION(bool, misc_spec_list, false);
	OPTION(bool, misc_thirdperson, false);
	OPTION(int, misc_thirdperson_dist, 72);
	OPTION(int, misc_thirdperson_key, 0);
	OPTION(bool, misc_ragdoll_launcher, false);
	OPTION(float, misc_ragdoll_launcher_force, 2.f);
	OPTION(bool, misc_quickswitch, false);
	OPTION(bool, misc_remove_scope, false);
	OPTION(bool, misc_walkbot, false);
	OPTION(bool, misc_walkbot_walk, false);
	OPTION(bool, misc_walkbot_adaptive, false);
	OPTION(int, misc_walkbot_add_key, 0);
	OPTION(int, misc_walkbot_erase_key, 0);
	OPTION(bool, misc_hide_names, false);
	OPTION(bool, misc_anti_votekick, false);
	OPTION(bool, misc_circle_strafe, false);
	OPTION(int, misc_circle_strafe_on_key, 0);
	OPTION(bool, misc_rank_reveal, false);

	OPTION(bool, misc_rcs, false);
	OPTION(float, misc_rcs_amount, 2.0);
	OPTION(int, misc_config, 0);
	OPTION(bool, misc_predict_enemy_lby, false);
	OPTION(bool, misc_resolver, false);
	OPTION(int, misc_resolver_type, 0);
	OPTION(bool, misc_chatspam, false);
	OPTION(bool, misc_sonar, false);
	OPTION(bool, misc_radar, false);
	OPTION(bool, misc_no_flash, false);
	OPTION(bool, misc_nightmode, false);
	OPTION(float, misc_nightmode_normal_val, 0.05);
	OPTION(float, misc_nightmode_staticprop_val, 0.28);
	OPTION(bool, misc_asus, false);
	OPTION(float, misc_asus_normal_val, 0.05);
	OPTION(float, misc_asus_staticprop_val, 0.28);
	OPTION(bool, misc_fast_duck, false);
	OPTION(bool, misc_custom_models, false);
	OPTION(bool, misc_hide_ambient_sounds, false);
	OPTION(bool, misc_menu_sounds, true);
	OPTION(bool, misc_menu_snow, false);
	OPTION(bool, misc_auto_derank, false);
	OPTION(bool, misc_silent_walk, false);
	OPTION(bool, misc_copy_bot, false);
	
	OPTION(bool, backtrack_enabled, false);
	OPTION(bool, backtrack_lc_break, false);
	OPTION(bool, backtrack_chams, false);
	OPTION(bool, backtrack_chams_lc_break, false);
	OPTION(int, backtrack_chams_style, 0);
	OPTION(int, backtrack_ticks, 12);
	OPTION(int, backtrack_on_key, 0);
	OPTION(float, backtrack_dist, 64);
	OPTION(bool, backtrack_skeleton, false);

	OPTION(bool, skinchanger_enabled, false);

	// 
	// COLORS
	// 
	//first 0 is team, 1 is enemy, 2 is local. 
	//second 0 is visible, 1 is occluded.
	//same goes for 1d arrays, 0 is team, 1 is enemy, 2 is local
	using twoD_array = std::array<std::array<Color, 2>, 3>;
	using oneD_array = std::array<Color, 3>;

	twoD_array color_esp = { {{Color(0, 128, 255), Color(0, 128, 255)}, {Color(0, 128, 255), Color(0, 128, 255)}, {Color(0, 128, 255), Color(0, 128, 255)} } };

	OPTION(Color, color_esp_c4, Color(255, 255, 0));

	twoD_array color_chams = { { {Color(0, 128, 255), Color(0, 128, 255)}, {Color(0, 128, 255), Color(0, 128, 255)}, {Color(0, 128, 255), Color(0, 128, 255)} } };

	twoD_array color_chams_backtrack = {{ {Color(0, 128, 255), Color(0, 128, 255)}, {Color(0, 128, 255), Color(0, 128, 255)}, {Color(0, 128, 255), Color(0, 128, 255)} }};
	oneD_array color_backtrack_skeleton = { Color(255, 0, 0),Color(255, 0, 0),Color(255, 0, 0) };

	oneD_array color_footstep_beams = { Color(255, 0, 0),Color(255, 0, 0),Color(255, 0, 0) };

	OPTION(Color, color_esp_crosshair, Color(255, 255, 255));

	OPTION(Color, color_hitmarker_text, Color(255, 255, 0));
	OPTION(Color, color_hitmarker, Color(255, 255, 255));

	oneD_array color_bullet_beams = { Color(255, 255, 255), Color(255, 255, 255), Color(255, 255, 255) };

	
	twoD_array color_glow = { { {Color(0, 128, 255), Color(0, 128, 255)}, {Color(0, 128, 255), Color(0, 128, 255)}, {Color(0, 128, 255), Color(0, 128, 255)} } };

	OPTION(Color, color_glow_others_weapons, Color(0, 128, 255));
	OPTION(Color, color_glow_c4, Color(0, 255, 128));

	OPTION(Color, color_beam_smoke, Color(0, 255, 128));
	OPTION(Color, color_beam_molotov, Color(0, 255, 128));
	OPTION(Color, color_beam_decoy, Color(0, 255, 128));

	OPTION(Color, color_aimbot_target_text, Color(0, 255, 128));
	
	OPTION(Color, color_esp_hurt_hitboxes, Color(0, 255, 128));

	OPTION(Color, color_spread_crosshair, Color(0, 255, 128));

	OPTION(Color, color_offscreen_esp, Color(255, 255, 255));
	
	oneD_array color_bullet_impacts = { Color(255,255,255, 255), Color(255,255,255, 255), Color(255,255,255, 255) };

	OPTION(Color, color_taser_range, Color(255, 255, 255));

	OPTION(Color, color_hud_feature_text, Color(255, 255, 255));
	OPTION(Color, color_hud_feature_box, Color(135, 135, 135, 70));

	OPTION(Color, color_origin_box, Color(135, 135, 135, 70));

	OPTION(Color, color_walkbot_next, Color(0, 255, 0, 150));
	OPTION(Color, color_walkbot_etc, Color(0, 0, 170, 150));

	OPTION(Color, color_fov_ring, Color(0, 0, 170, 150));

	oneD_array color_viewangles = { Color(170, 0, 0, 150),Color(170, 0, 0, 150),Color(170, 0, 0, 150) };

	OPTION(Color, color_ammo_bar, Color(0, 0, 170, 150));

	OPTION(Color, color_autowall_crosshair, Color(0, 0, 170, 150));

	OPTION(Color, color_defusing_bar, Color(0, 0, 170, 150));
	OPTION(Color, color_planting_bar, Color(0, 0, 170, 150));
	OPTION(Color, color_dangerzone_items, Color(0, 0, 170, 150));

};

extern Config g_vars;
extern bool   g_Unload;
extern bool   g_Save;
extern bool   g_Load;
extern bool   g_Clear;
extern bool   g_View;
