#pragma once
#include "json.h"
#include "valve_sdk\csgostructs.hpp"
#include "options.hpp"
#include "json_configs.h"
#include <unordered_map>
#include <array>
#include "lists.h"
#include "skinchanger.h"
#include "local_info.h"
#include "http_client.h"
#include <filesystem>
#include "notification.h"

namespace Configs
{

	void get_val(Json::Value& config, int* setting)
	{
		if (config.isNull())
			return;

		*setting = config.asInt();
	}

	void get_val(Json::Value& config, bool* setting)
	{
		if (config.isNull())
			return;

		*setting = config.asBool();
	}

	void get_val(Json::Value& config, float* setting)
	{
		if (config.isNull())
			return;

		*setting = config.asFloat();
	}

	void get_val(Json::Value& config, Color* setting)
	{
		if (config.isNull())
			return;

		static int R, G, B, A;

		get_val(config["R"], &R);
		get_val(config["G"], &G);
		get_val(config["B"], &B);
		get_val(config["A"], &A);

		setting->SetColor(R, G, B, A);
	}

	void get_val(Json::Value& config, char** setting)
	{
		if (config.isNull())
			return;

		*setting = strdup(config.asCString());
	}

	void get_val(Json::Value& config, char* setting)
	{
		if (config.isNull())
			return;

		strcpy(setting, config.asCString());
	}

	void load_color(Json::Value& config, Color* color)
	{
		static int R, G, B, A;

		color->GetColor(R, G, B, A);

		config["R"] = R;
		config["G"] = G;
		config["B"] = B;
		config["A"] = A;
	}

	std::string get_team(int num)
	{
		switch (num % 3)
		{
		case 0: return "team"; break;
		case 1: return "enemy"; break;
		case 2: return "local"; break;
		};
		return "";
	}

	std::string get_visibility(int num)
	{
		switch (num % 2)
		{
		case 0: return "visible"; break;
		case 1: return "occluded"; break;
		}
		return "";
	}

	Json::Value get_settings()
	{
		Json::Value settings;

		settings["aimbot"]["enabled"] = g_vars.aimbot_enabled;

		for (int i = 0; i < 2; i++)
		{
			settings["aimbot"]["aimbot_target_" + get_team(i)] = g_vars.aimbot_targets[i];
		}

		settings["aimbot"]["vis_check"] = g_vars.aimbot_vis_check;
		settings["aimbot"]["autowall"] = g_vars.aimbot_autowall;
		settings["aimbot"]["autowall_dmg"] = g_vars.aimbot_autowall_dmg;
		settings["aimbot"]["autoshoot"] = g_vars.aimbot_autoshoot_enabled;
		settings["aimbot"]["autoscope"] = g_vars.aimbot_autoscope_enabled;
		settings["aimbot"]["silent_aim"] = g_vars.aimbot_silent_aim;
		settings["aimbot"]["on_attack"] = g_vars.aimbot_on_attack;
		settings["aimbot"]["key"] = g_vars.aimbot_on_key;
		settings["aimbot"]["shots"] = g_vars.aimbot_aim_after_shots;
		settings["aimbot"]["aim_chance"] = g_vars.aimbot_aim_chance;
		settings["aimbot"]["aim_chance_percent"] = g_vars.aimbot_aim_chance_percent;
		settings["aimbot"]["jumping"] = g_vars.aimbot_avoid_jumping;
		settings["aimbot"]["smoke_check"] = g_vars.aimbot_smoke_check;
		settings["aimbot"]["flash_check"] = g_vars.aimbot_flash_check;
		settings["aimbot"]["fov"] = g_vars.aimbot_fov;
		settings["aimbot"]["smooth"] = g_vars.aimbot_smooth;
		settings["aimbot"]["hitbox"] = g_vars.aimbot_hitbox;
		settings["aimbot"]["cooldown"] = g_vars.aimbot_target_cooldown;
		settings["aimbot"]["cooldown_time"] = g_vars.aimbot_target_time;
		settings["aimbot"]["speed_limit"] = g_vars.aimbot_speed_limit;
		settings["aimbot"]["speed"] = g_vars.aimbot_speed;
		settings["aimbot"]["get_best_hitbox"] = g_vars.aimbot_get_best_hitbox;
		settings["aimbot"]["get_best_backtrack_tick"] = g_vars.aimbot_get_best_backtrack;
		settings["aimbot"]["reaction"] = g_vars.aimbot_reaction;
		settings["aimbot"]["reaction_time"] = g_vars.aimbot_reaction_time;


		//trigger
		settings["trigger"]["enabled"] = g_vars.triggerbot_enabled;
		for (int i = 0; i < 2; i++)
		{
			settings["trigger"]["shoot_at_" + get_team(i)] = g_vars.triggerbot_targets[i];
		}
		settings["trigger"]["at_backtrack"] = g_vars.triggerbot_backtrack;
		settings["trigger"]["key"] = g_vars.triggerbot_key;
		settings["trigger"]["delay"] = g_vars.triggerbot_delay;
		for (size_t i = 0; i < menu_lists::hitboxes_shortened.size(); i++)
		{
			settings["trigger"]["hitbox_" + menu_lists::hitboxes_shortened.at(i)] = g_vars.triggerbot_hitboxes[i];
		}

		//esp
		settings["esp"]["enabled"] = g_vars.esp_enabled;
		settings["esp"]["key"] = g_vars.esp_on_key;
		for (int i = 0; i < 3; i++)
		{
			settings["esp"][get_team(i)] = g_vars.esp_targets[i];
		}
		settings["esp"]["boxes"] = g_vars.esp_player_boxes;
		settings["esp"]["names"] = g_vars.esp_player_names;
		settings["esp"]["health"] = g_vars.esp_player_health;
		settings["esp"]["health_thick"] = g_vars.esp_player_health_thick;
		settings["esp"]["health_fractional"] = g_vars.esp_player_health_fractional;
		settings["esp"]["armor"] = g_vars.esp_player_armour;
		settings["esp"]["armor_thick"] = g_vars.esp_player_armour_thick;
		settings["esp"]["armor_fractional"] = g_vars.esp_player_armour_fractional;
		settings["esp"]["weapons"] = g_vars.esp_player_weapons;
		settings["esp"]["snaplines"] = g_vars.esp_player_snaplines;
		settings["esp"]["crosshair"] = g_vars.esp_crosshair;
		settings["esp"]["planted_c4"] = g_vars.esp_planted_c4;
		settings["esp"]["footstep_beams"] = g_vars.vis_misc_footstep_beams;
		settings["esp"]["bullet_beams"] = g_vars.vis_misc_bullet_beams;
		settings["esp"]["bullet_beams_type"] = g_vars.vis_misc_bullet_beams_type;
		for (int i = 0; i < 3; i++)
		{
			settings["esp"]["bullet_beams_" + get_team(i)] = g_vars.vis_misc_bullet_beams_targets[i];
		}
		settings["esp"]["bullet_beams_lifetime"] = g_vars.vis_misc_bullet_beams_lifetime;
		settings["esp"]["bullet_beams_size"] = g_vars.vis_misc_bullet_beams_size;
		settings["esp"]["hitmarkers"] = g_vars.vis_misc_hitmarkers;
		settings["esp"]["hitmarkers_style"] = g_vars.vis_misc_hitmarkers_style;
		settings["esp"]["hitmarkers_bt_tick"] = g_vars.vis_misc_hitmarkers_bt_tick;
		settings["esp"]["hitmarkers_show_hitbox"] = g_vars.vis_misc_hitmarkers_hitbox;
		settings["esp"]["vulnerabilities"] = g_vars.esp_player_vulnerabilities;
		settings["esp"]["aimbot_target"] = g_vars.esp_aimbot_target;
		settings["esp"]["hurt_hitboxes"] = g_vars.esp_hurt_hitboxes;
		settings["esp"]["hurt_hitboxes_duration"] = g_vars.esp_hurt_hitboxes_time;
		settings["esp"]["farther"] = g_vars.esp_farther;
		settings["esp"]["offscreen"] = g_vars.esp_offscreen;
		settings["esp"]["offscreen_triangle_width"] = g_vars.esp_offscreen_width;
		settings["esp"]["offscreen_triangle_height"] = g_vars.esp_offscreen_height;
		settings["esp"]["bullet_impacts"] = g_vars.vis_misc_bullet_impacts;
		for (int i = 0; i < 3; i++)
		{
			settings["esp"]["bullet_impacts_" + get_team(i)] = g_vars.vis_misc_bullet_impacts_targets[i];
		}
		settings["esp"]["bullet_impacts_lifetime"] = g_vars.vis_misc_bullet_impacts_lifetime;
		settings["esp"]["bullet_impacts_size"] = g_vars.vis_misc_bullet_impacts_size;
		settings["esp"]["origin_box"] = g_vars.esp_origin_box;
		settings["esp"]["origin_box_size"] = g_vars.esp_origin_box_size;
		settings["esp"]["origin_box_style"] = g_vars.esp_origin_box_style;
		settings["esp"]["money"] = g_vars.esp_player_money;
		settings["esp"]["viewangles"] = g_vars.esp_player_viewangles;
		settings["esp"]["viewangles_length"] = g_vars.esp_viewangles_length;
		settings["esp"]["distance"] = g_vars.esp_distance;
		settings["esp"]["distance_style"] = g_vars.esp_distance_style;
		settings["esp"]["ammo_bar"] = g_vars.esp_player_ammo_bar;
		settings["esp"]["skeleton"] = g_vars.esp_player_skeleton;
		settings["esp"]["bomb_carrier"] = g_vars.esp_player_c4;
		settings["esp"]["has_defuse_kit"] = g_vars.esp_player_defuser;
		settings["esp"]["bomb_planting_bar"] = g_vars.esp_player_planting_bar;
		settings["esp"]["bomb_defusing_bar"] = g_vars.esp_player_defusing_bar;
		settings["esp"]["draw_vs_user"] = g_vars.esp_draw_vs_user;
		settings["esp"]["purchase_listener"] = g_vars.vis_misc_log_purchases;
		settings["esp"]["dz_item_esp"] = g_vars.esp_draw_dangerzone_items;
		settings["esp"]["draw_dist"] = g_vars.esp_drawing_dist;

		settings["glow"]["enabled"] = g_vars.glow_enabled;
		settings["glow"]["key"] = g_vars.glow_on_key;
		settings["glow"]["players"] = g_vars.glow_players;
		settings["glow"]["others"] = g_vars.glow_others;
		settings["glow"]["others_style"] = g_vars.glow_others_style;

		settings["misc"]["bhop"] = g_vars.misc_bhop;
		settings["misc"]["bhop_chance"] = g_vars.misc_bhop_chance;
		settings["misc"]["jumps_tick"] = g_vars.misc_jumps_per_tick;
		settings["misc"]["autostrafe"] = g_vars.misc_autostrafe;
		settings["misc"]["autostrafe_type"] = g_vars.misc_autostrafe_type;
		settings["misc"]["edge_jump"] = g_vars.misc_edge_jump;
		settings["misc"]["edge_jump_crouch"] = g_vars.misc_edge_jump_crouch;

		settings["misc"]["legit_aa"] = g_vars.misc_legit_aa;
		settings["misc"]["legit_aa_slide_fix"] = g_vars.misc_legit_aa_slide_fix;
		settings["misc"]["legit_aa_ang"] = g_vars.misc_legit_aa_ang;
		settings["misc"]["legit_aa_break_lby"] = g_vars.misc_legit_aa_break_lby;
		settings["misc"]["legit_aa_break_lby_dist"] = g_vars.misc_legit_aa_break_lby_dist;
		settings["misc"]["legit_aa_fakelag"] = g_vars.misc_legit_aa_fakelag;
		settings["misc"]["legit_aa_fakelag_amount"] = g_vars.misc_legit_aa_fakelag_amount;
		settings["misc"]["legit_aa_pitch"] = g_vars.misc_legit_aa_pitch;
		settings["misc"]["legit_aa_pitch_val"] = g_vars.misc_legit_aa_pitch_val;

		settings["misc"]["clantag_changer"] = g_vars.misc_clantag_changer;
		settings["misc"]["spec_list"] = g_vars.misc_spec_list;
		settings["misc"]["third_person"] = g_vars.misc_thirdperson;
		settings["misc"]["thirdperson_dist"] = g_vars.misc_thirdperson_dist;
		settings["misc"]["thirdperson_key"] = g_vars.misc_thirdperson_key;
		settings["misc"]["rcs"] = g_vars.misc_rcs;
		settings["misc"]["rcs_amount"] = g_vars.misc_rcs_amount;
		settings["misc"]["chatspam"] = g_vars.misc_chatspam;
		settings["misc"]["sonar"] = g_vars.misc_sonar;
		settings["misc"]["radar"] = g_vars.misc_radar;
		settings["misc"]["no_flash"] = g_vars.misc_no_flash;
		settings["misc"]["skybox_number"] = g_vars.vis_misc_skybox;
		settings["misc"]["no_smoke"] = g_vars.vis_misc_no_smoke;
		settings["misc"]["spread_crosshair"] = g_vars.vis_misc_spread_crosshair;
		settings["misc"]["ragdoll_launcher"] = g_vars.misc_ragdoll_launcher;
		settings["misc"]["ragdoll_launcher_multiplier"] = g_vars.misc_ragdoll_launcher_force;
		settings["misc"]["quickswitch_after_shot"] = g_vars.misc_quickswitch;
		settings["misc"]["taser_range"] = g_vars.vis_misc_taser_range;
		settings["misc"]["grenade_circles"] = g_vars.vis_misc_grenade_circle;
		settings["misc"]["override_fov"] = g_vars.vis_misc_override_fov;
		settings["misc"]["override_viewmodel_fov"] = g_vars.vis_misc_override_viewmodel_fov;
		settings["misc"]["draw_grenade_path"] = g_vars.vis_misc_draw_grenade_path;
		settings["misc"]["infinite_duck"] = g_vars.misc_fast_duck;
		settings["misc"]["draw_hud_features"] = g_vars.vis_misc_hud_features;
		settings["misc"]["draw_hud_features_box"] = g_vars.vis_misc_hud_features_box;
		settings["misc"]["remove_scope"] = g_vars.misc_remove_scope;
		settings["misc"]["fov_ring"] = g_vars.vis_misc_fov_ring;
		settings["misc"]["custom_models"] = g_vars.misc_custom_models;
		settings["misc"]["hide_names"] = g_vars.misc_hide_names;
		settings["misc"]["anti_votekick"] = g_vars.misc_anti_votekick;
		settings["misc"]["autowall_damage"] = g_vars.vis_misc_autowall_crosshair;
		settings["misc"]["hide_ambient_sounds"] = g_vars.misc_hide_ambient_sounds;
		settings["misc"]["menu_sounds"] = g_vars.misc_menu_sounds;
		settings["misc"]["menu_snow"] = g_vars.misc_menu_snow;
		settings["misc"]["nightmode"] = g_vars.misc_nightmode;
		settings["misc"]["nightmode_general"] = g_vars.misc_nightmode_normal_val;
		settings["misc"]["nightmode_props"] = g_vars.misc_nightmode_staticprop_val;
		settings["misc"]["asus"] = g_vars.misc_asus;
		settings["misc"]["asus_general"] = g_vars.misc_asus_normal_val;
		settings["misc"]["asus_props"] = g_vars.misc_asus_staticprop_val;
		settings["misc"]["auto_derank"] = g_vars.misc_auto_derank;
		settings["misc"]["notifications_hurt"] = g_vars.vis_misc_hurt_notifications;
		settings["misc"]["rank_reveal"] = g_vars.misc_rank_reveal;
		settings["misc"]["silent_walk"] = g_vars.misc_silent_walk;
		settings["misc"]["copy_bot"] = g_vars.misc_copy_bot;
		
		settings["walkbot"]["enabled"] = g_vars.misc_walkbot;
		settings["walkbot"]["walk"] = g_vars.misc_walkbot_walk;
		settings["walkbot"]["adaptive"] = g_vars.misc_walkbot_adaptive;
		settings["walkbot"]["add_key"] = g_vars.misc_walkbot_add_key;
		settings["walkbot"]["erase_key"] = g_vars.misc_walkbot_erase_key;

		settings["backtrack"]["enabled"] = g_vars.backtrack_enabled;
		settings["backtrack"]["skip_lc_breaker"] = g_vars.backtrack_lc_break;
		settings["backtrack"]["ticks"] = g_vars.backtrack_ticks;
		settings["backtrack"]["max_dist"] = g_vars.backtrack_dist;
		settings["backtrack"]["key"] = g_vars.backtrack_on_key;
		settings["backtrack"]["chams"] = g_vars.backtrack_chams;
		settings["backtrack"]["chams_styles"] = g_vars.backtrack_chams_style;
		settings["backtrack"]["chams_skip_lc_breaker"] = g_vars.backtrack_chams_lc_break;
		settings["backtrack"]["skeleton"] = g_vars.backtrack_skeleton;

		settings["resolver"]["predict_lby_update"] = g_vars.misc_predict_enemy_lby;
		settings["resolver"]["legit_aa_normal"] = g_vars.misc_resolver;
		settings["resolver"]["legit_aa_resolver_type"] = g_vars.misc_resolver_type;

		settings["chams"]["enabled"] = g_vars.chams_enabled;
		for (int i = 0; i < 3; i++)
		{
			settings["chams"][get_team(i)] = g_vars.chams_targets[i];
		}
		settings["chams"]["ignorez"] = g_vars.chams_ignorez;
		settings["chams"]["wireframe"] = g_vars.chams_wireframe;
		settings["chams"]["flat"] = g_vars.chams_flat;
		settings["chams"]["glass"] = g_vars.chams_glass;
		settings["chams"]["metallic"] = g_vars.chams_reflective;

		settings["skinchanger"]["enabled"] = g_vars.skinchanger_enabled;

		for (int i = 0; i < menu_lists::weapon_list_names.size(); i++)
		{
			settings["skinchanger"]["skin_" + menu_lists::weapon_list.at(i).first + "_skin_id"] = skinchanger.info[menu_lists::weapon_list.at(i).second].skin_id;
			settings["skinchanger"]["skin_" + menu_lists::weapon_list.at(i).first + "_wear"] = skinchanger.info[menu_lists::weapon_list.at(i).second].wear;
			settings["skinchanger"]["skin_" + menu_lists::weapon_list.at(i).first + "_seed"] = skinchanger.info[menu_lists::weapon_list.at(i).second].seed;
		}

		for (int i = 0; i < 3; i++)
		{
			for (int v = 0; v < 2; v++)
			{
				//prevents logging localplayer occluded, which never happens
				if (i == 2 && v == 1)
					continue;

				load_color(settings["color"]["esp_" + get_team(i) + "_" + get_visibility(v)], &g_vars.color_esp[i][v]);
			}
		}

		load_color(settings["color"]["esp_c4"], &g_vars.color_esp_c4);



		for (int i = 0; i < 3; i++)
		{
			for (int v = 0; v < 2; v++)
			{
				//prevents logging localplayer occluded, which never happens
				if (i == 2 && v == 1)
					continue;

				load_color(settings["color"]["chams_" + get_team(i) + "_" + get_visibility(v)], &g_vars.color_chams[i][v]);
			}
		}

		for (int i = 0; i < 3; i++)
		{
			for (int v = 0; v < 2; v++)
			{
				//prevents logging localplayer occluded, which never happens
				if (i == 2 && v == 1)
					continue;

				load_color(settings["color"]["chams_backtrack_" + get_team(i) + "_" + get_visibility(v)], &g_vars.color_chams_backtrack[i][v]);
			}
		}

		for (int i = 0; i < 3; i++)
		{
			load_color(settings["color"]["footstep_beams_" + get_team(i)], &g_vars.color_footstep_beams[i]);
		}

		load_color(settings["color"]["hitmarker"], &g_vars.color_hitmarker);
		load_color(settings["color"]["hitmarker_text"], &g_vars.color_hitmarker_text);

		for (int i = 0; i < 3; i++)
		{
			load_color(settings["color"]["bullet_beam_" + get_team(i)], &g_vars.color_bullet_beams[i]);
		}

		for (int i = 0; i < 2; i++)
		{
			for (int v = 0; v < 2; v++)
			{

				load_color(settings["color"]["glow_" + get_team(i) + "_" + get_visibility(v)], &g_vars.color_glow[i][v]);
			}
		}

		load_color(settings["color"]["glow_weapons"], &g_vars.color_glow_others_weapons);
		load_color(settings["color"]["glow_c4"], &g_vars.color_glow_c4);

		load_color(settings["color"]["beam_smoke"], &g_vars.color_beam_smoke);
		load_color(settings["color"]["beam_molotov"], &g_vars.color_beam_molotov);
		load_color(settings["color"]["beam_decoy"], &g_vars.color_beam_decoy);

		load_color(settings["color"]["aimbot_target_text"], &g_vars.color_aimbot_target_text);

		load_color(settings["color"]["hurt_hitboxes"], &g_vars.color_esp_hurt_hitboxes);

		load_color(settings["color"]["spread_crosshair"], &g_vars.color_spread_crosshair);

		load_color(settings["color"]["offscreen_esp"], &g_vars.color_offscreen_esp);

		for (int i = 0; i < 3; i++)
		{
			load_color(settings["color"]["bullet_impact_" + get_team(i)], &g_vars.color_bullet_impacts[i]);
		}

		load_color(settings["color"]["taser_range"], &g_vars.color_taser_range);

		load_color(settings["color"]["hud_features_text"], &g_vars.color_hud_feature_text);
		load_color(settings["color"]["hud_features_box"], &g_vars.color_hud_feature_box);

		load_color(settings["color"]["origin_box"], &g_vars.color_origin_box);

		load_color(settings["color"]["walkbot_next"], &g_vars.color_walkbot_next);
		load_color(settings["color"]["walkbot_etc"], &g_vars.color_walkbot_etc);

		load_color(settings["color"]["fov_ring"], &g_vars.color_fov_ring);
		for (int i = 0; i < 3; i++)
		{
			load_color(settings["color"]["viewangles_" + get_team(i)], &g_vars.color_viewangles[i]);
		}

		load_color(settings["color"]["autowall_crosshair"], &g_vars.color_autowall_crosshair);

		load_color(settings["color"]["planting_bar"], &g_vars.color_planting_bar);
		load_color(settings["color"]["defusing_bar"], &g_vars.color_defusing_bar);
		load_color(settings["color"]["dz_item_esp"], &g_vars.color_dangerzone_items);

		for (int i = 0; i < 3; i++)
		{
			load_color(settings["color"]["backtrack_skeleton" + get_team(i)], &g_vars.color_backtrack_skeleton[i]);
		}

		return settings;
	}

	void load_cfg_internal(Json::Value settings)
	{
		get_val(settings["aimbot"]["enabled"], &g_vars.aimbot_enabled);
		for (int i = 0; i < 2; i++)
		{
			get_val(settings["aimbot"]["aimbot_target_" + get_team(i)], &g_vars.aimbot_targets[i]);
		}
		get_val(settings["aimbot"]["vis_check"], &g_vars.aimbot_vis_check);
		get_val(settings["aimbot"]["autowall"], &g_vars.aimbot_autowall);
		get_val(settings["aimbot"]["autowall_dmg"], &g_vars.aimbot_autowall_dmg);
		get_val(settings["aimbot"]["autoshoot"], &g_vars.aimbot_autoshoot_enabled);
		get_val(settings["aimbot"]["autoscope"], &g_vars.aimbot_autoscope_enabled);
		get_val(settings["aimbot"]["silent_aim"], &g_vars.aimbot_silent_aim);
		get_val(settings["aimbot"]["on_attack"], &g_vars.aimbot_on_attack);
		get_val(settings["aimbot"]["key"], &g_vars.aimbot_on_key);
		get_val(settings["aimbot"]["shots"], &g_vars.aimbot_aim_after_shots);
		get_val(settings["aimbot"]["aim_chance"], &g_vars.aimbot_aim_chance);
		get_val(settings["aimbot"]["aim_chance_percent"], &g_vars.aimbot_aim_chance_percent);
		get_val(settings["aimbot"]["jumping"], &g_vars.aimbot_avoid_jumping);
		get_val(settings["aimbot"]["smoke_check"], &g_vars.aimbot_smoke_check);
		get_val(settings["aimbot"]["flash_check"], &g_vars.aimbot_flash_check);
		get_val(settings["aimbot"]["fov"], &g_vars.aimbot_fov);
		get_val(settings["aimbot"]["smooth"], &g_vars.aimbot_smooth);
		get_val(settings["aimbot"]["hitbox"], &g_vars.aimbot_hitbox);
		get_val(settings["aimbot"]["cooldown"], &g_vars.aimbot_target_cooldown);
		get_val(settings["aimbot"]["cooldown_time"], &g_vars.aimbot_target_time);
		get_val(settings["aimbot"]["speed_limit"], &g_vars.aimbot_speed_limit);
		get_val(settings["aimbot"]["speed"], &g_vars.aimbot_speed);
		get_val(settings["aimbot"]["get_best_hitbox"], &g_vars.aimbot_get_best_hitbox);
		get_val(settings["aimbot"]["get_best_backtrack_tick"], &g_vars.aimbot_get_best_backtrack);
		get_val(settings["aimbot"]["reaction"], &g_vars.aimbot_reaction);
		get_val(settings["aimbot"]["reaction_time"], &g_vars.aimbot_reaction_time);

		//triggerbot
		get_val(settings["trigger"]["enabled"], &g_vars.triggerbot_enabled);
		for (int i = 0; i < 2; i++)
		{
			get_val(settings["trigger"]["shoot_at_" + get_team(i)], &g_vars.triggerbot_targets[i]);
		}
		get_val(settings["trigger"]["at_backtrack"], &g_vars.triggerbot_backtrack);
		get_val(settings["trigger"]["key"], &g_vars.triggerbot_key);
		get_val(settings["trigger"]["delay"], &g_vars.triggerbot_delay);
		for (size_t i = 0; i < menu_lists::hitboxes_shortened.size(); i++)
		{
			get_val(settings["trigger"]["hitbox_" + menu_lists::hitboxes_shortened.at(i)], &g_vars.triggerbot_hitboxes[i]);
		}

		//esp
		get_val(settings["esp"]["enabled"], &g_vars.esp_enabled);
		get_val(settings["esp"]["key"], &g_vars.esp_on_key);
		for (int i = 0; i < 3; i++)
		{
			get_val(settings["esp"][get_team(i)], &g_vars.esp_targets[i]);
		}
		get_val(settings["esp"]["boxes"], &g_vars.esp_player_boxes);
		get_val(settings["esp"]["names"], &g_vars.esp_player_names);
		get_val(settings["esp"]["health"], &g_vars.esp_player_health);
		get_val(settings["esp"]["health_thick"], &g_vars.esp_player_health_thick);
		get_val(settings["esp"]["health_fractional"], &g_vars.esp_player_health_fractional);
		get_val(settings["esp"]["armor"], &g_vars.esp_player_armour);
		get_val(settings["esp"]["armor_thick"], &g_vars.esp_player_armour_thick);
		get_val(settings["esp"]["armor_fractional"], &g_vars.esp_player_armour_fractional);
		get_val(settings["esp"]["weapons"], &g_vars.esp_player_weapons);
		get_val(settings["esp"]["snaplines"], &g_vars.esp_player_snaplines);
		get_val(settings["esp"]["crosshair"], &g_vars.esp_crosshair);
		get_val(settings["esp"]["planted_c4"], &g_vars.esp_planted_c4);
		get_val(settings["esp"]["footstep_beams"], &g_vars.vis_misc_footstep_beams);
		get_val(settings["esp"]["bullet_beams"], &g_vars.vis_misc_bullet_beams);
		get_val(settings["esp"]["bullet_beams_type"], &g_vars.vis_misc_bullet_beams_type);

		for (int i = 0; i < 3; i++)
		{
			get_val(settings["esp"]["bullet_beams_" + get_team(i)], &g_vars.vis_misc_bullet_beams_targets[i]);
		}

		for (int i = 0; i < 3; i++)
		{
			get_val(settings["esp"]["bullet_impacts_" + get_team(i)], &g_vars.vis_misc_bullet_impacts_targets[i]);
		}
		get_val(settings["esp"]["bullet_beams_lifetime"], &g_vars.vis_misc_bullet_beams_lifetime);
		get_val(settings["esp"]["bullet_beams_size"], &g_vars.vis_misc_bullet_beams_size);
		get_val(settings["esp"]["hitmarkers"], &g_vars.vis_misc_hitmarkers);
		get_val(settings["esp"]["hitmarkers_style"], &g_vars.vis_misc_hitmarkers_style);
		get_val(settings["esp"]["hitmarkers_bt_tick"], &g_vars.vis_misc_hitmarkers_bt_tick);
		get_val(settings["esp"]["hitmarkers_show_hitbox"], &g_vars.vis_misc_hitmarkers_hitbox);
		get_val(settings["esp"]["vulnerabilities"], &g_vars.esp_player_vulnerabilities);
		get_val(settings["esp"]["aimbot_target"], &g_vars.esp_aimbot_target);
		get_val(settings["esp"]["hurt_hitboxes"], &g_vars.esp_hurt_hitboxes);
		get_val(settings["esp"]["hurt_hitboxes_duration"], &g_vars.esp_hurt_hitboxes_time);
		get_val(settings["esp"]["farther"], &g_vars.esp_farther);
		get_val(settings["esp"]["offscreen"], &g_vars.esp_offscreen);
		get_val(settings["esp"]["offscreen_triangle_width"], &g_vars.esp_offscreen_width);
		get_val(settings["esp"]["offscreen_triangle_height"], &g_vars.esp_offscreen_height);
		get_val(settings["esp"]["bullet_impacts"], &g_vars.vis_misc_bullet_impacts);
		for (int i = 0; i < 3; i++)
		{
			get_val(settings["esp"]["bullet_impacts_" + get_team(i)], &g_vars.vis_misc_bullet_impacts_targets[i]);
		}
		get_val(settings["esp"]["bullet_impacts_lifetime"], &g_vars.vis_misc_bullet_impacts_lifetime);
		get_val(settings["esp"]["bullet_impacts_size"], &g_vars.vis_misc_bullet_impacts_size);
		get_val(settings["esp"]["origin_box"], &g_vars.esp_origin_box);
		get_val(settings["esp"]["origin_box_size"], &g_vars.esp_origin_box_size);
		get_val(settings["esp"]["origin_box_style"], &g_vars.esp_origin_box_style);
		get_val(settings["esp"]["money"], &g_vars.esp_player_money);
		get_val(settings["esp"]["viewangles"], &g_vars.esp_player_viewangles);
		get_val(settings["esp"]["viewangles_length"], &g_vars.esp_viewangles_length);
		get_val(settings["esp"]["distance"], &g_vars.esp_distance);
		get_val(settings["esp"]["distance_style"], &g_vars.esp_distance_style);
		get_val(settings["esp"]["ammo_bar"], &g_vars.esp_player_ammo_bar);
		get_val(settings["esp"]["skeleton"], &g_vars.esp_player_skeleton);
		get_val(settings["esp"]["bomb_carrier"], &g_vars.esp_player_c4);
		get_val(settings["esp"]["has_defuse_kit"], &g_vars.esp_player_defuser);
		get_val(settings["esp"]["bomb_planting_bar"], &g_vars.esp_player_planting_bar);
		get_val(settings["esp"]["bomb_defusing_bar"], &g_vars.esp_player_defusing_bar);
		get_val(settings["esp"]["draw_vs_user"], &g_vars.esp_draw_vs_user);
		get_val(settings["esp"]["purchase_listener"], &g_vars.vis_misc_log_purchases);
		get_val(settings["esp"]["dz_item_esp"], &g_vars.esp_draw_dangerzone_items);
		get_val(settings["esp"]["draw_dist"], &g_vars.esp_drawing_dist);

		get_val(settings["glow"]["enabled"], &g_vars.glow_enabled);
		get_val(settings["glow"]["key"], &g_vars.glow_on_key);
		get_val(settings["glow"]["players"], &g_vars.glow_players);
		get_val(settings["glow"]["others"], &g_vars.glow_others);
		get_val(settings["glow"]["others_style"], &g_vars.glow_others_style);

		get_val(settings["misc"]["bhop"], &g_vars.misc_bhop);
		get_val(settings["misc"]["bhop_chance"], &g_vars.misc_bhop_chance);
		get_val(settings["misc"]["jumps_tick"], &g_vars.misc_jumps_per_tick);
		get_val(settings["misc"]["autostrafe"], &g_vars.misc_autostrafe);
		get_val(settings["misc"]["autostrafe_type"], &g_vars.misc_autostrafe_type);
		get_val(settings["misc"]["edge_jump"], &g_vars.misc_edge_jump);
		get_val(settings["misc"]["edge_jump_crouch"], &g_vars.misc_edge_jump_crouch);

		get_val(settings["misc"]["legit_aa"], &g_vars.misc_legit_aa);
		get_val(settings["misc"]["legit_aa_slide_fix"], &g_vars.misc_legit_aa_slide_fix);
		get_val(settings["misc"]["legit_aa_ang"], &g_vars.misc_legit_aa_ang);
		get_val(settings["misc"]["legit_aa_break_lby"], &g_vars.misc_legit_aa_break_lby);
		get_val(settings["misc"]["legit_aa_break_lby_dist"], &g_vars.misc_legit_aa_break_lby_dist);
		get_val(settings["misc"]["legit_aa_fakelag"], &g_vars.misc_legit_aa_fakelag);
		get_val(settings["misc"]["legit_aa_fakelag_amount"], &g_vars.misc_legit_aa_fakelag_amount);
		get_val(settings["misc"]["legit_aa_pitch"], &g_vars.misc_legit_aa_pitch);
		get_val(settings["misc"]["legit_aa_pitch_val"], &g_vars.misc_legit_aa_pitch_val);

		get_val(settings["misc"]["clantag_changer"], &g_vars.misc_clantag_changer);
		get_val(settings["misc"]["spec_list"], &g_vars.misc_spec_list);
		get_val(settings["misc"]["third_person"], &g_vars.misc_thirdperson);
		get_val(settings["misc"]["thirdperson_dist"], &g_vars.misc_thirdperson_dist);
		get_val(settings["misc"]["thirdperson_key"], &g_vars.misc_thirdperson_key);
		get_val(settings["misc"]["rcs"], &g_vars.misc_rcs);
		get_val(settings["misc"]["rcs_amount"], &g_vars.misc_rcs_amount);
		get_val(settings["misc"]["chatspam"], &g_vars.misc_chatspam);
		get_val(settings["misc"]["sonar"], &g_vars.misc_sonar);
		get_val(settings["misc"]["radar"], &g_vars.misc_radar);
		get_val(settings["misc"]["no_flash"], &g_vars.misc_no_flash);
		get_val(settings["misc"]["skybox_number"], &g_vars.vis_misc_skybox);
		get_val(settings["misc"]["no_smoke"], &g_vars.vis_misc_no_smoke);
		get_val(settings["misc"]["spread_crosshair"], &g_vars.vis_misc_spread_crosshair);
		get_val(settings["misc"]["ragdoll_launcher"], &g_vars.misc_ragdoll_launcher);
		get_val(settings["misc"]["ragdoll_launcher_multiplier"], &g_vars.misc_ragdoll_launcher_force);
		get_val(settings["misc"]["quickswitch_after_shot"], &g_vars.misc_quickswitch);
		get_val(settings["misc"]["taser_range"], &g_vars.vis_misc_taser_range);
		get_val(settings["misc"]["grenade_circles"], &g_vars.vis_misc_grenade_circle);
		get_val(settings["misc"]["override_fov"], &g_vars.vis_misc_override_fov);
		get_val(settings["misc"]["override_viewmodel_fov"], &g_vars.vis_misc_override_viewmodel_fov);
		get_val(settings["misc"]["draw_grenade_path"], &g_vars.vis_misc_draw_grenade_path);
		get_val(settings["misc"]["infinite_duck"], &g_vars.misc_fast_duck);
		get_val(settings["misc"]["draw_hud_features"], &g_vars.vis_misc_hud_features);
		get_val(settings["misc"]["draw_hud_features_box"], &g_vars.vis_misc_hud_features_box);
		get_val(settings["misc"]["remove_scope"], &g_vars.misc_remove_scope);
		get_val(settings["misc"]["fov_ring"], &g_vars.vis_misc_fov_ring);
		get_val(settings["misc"]["custom_models"], &g_vars.misc_custom_models);
		get_val(settings["misc"]["hide_names"], &g_vars.misc_hide_names);
		get_val(settings["misc"]["anti_votekick"], &g_vars.misc_anti_votekick);
		get_val(settings["misc"]["autowall_damage"], &g_vars.vis_misc_autowall_crosshair);
		get_val(settings["misc"]["hide_ambient_sounds"], &g_vars.misc_hide_ambient_sounds);
		get_val(settings["misc"]["menu_sounds"], &g_vars.misc_menu_sounds);
		get_val(settings["misc"]["menu_snow"], &g_vars.misc_menu_snow);
		get_val(settings["misc"]["nightmode"], &g_vars.misc_nightmode);
		get_val(settings["misc"]["nightmode_general"], &g_vars.misc_nightmode_normal_val);
		get_val(settings["misc"]["nightmode_props"], &g_vars.misc_nightmode_staticprop_val);
		get_val(settings["misc"]["asus"], &g_vars.misc_asus);
		get_val(settings["misc"]["asus_general"], &g_vars.misc_asus_normal_val);
		get_val(settings["misc"]["asus_props"], &g_vars.misc_asus_staticprop_val);
		get_val(settings["misc"]["auto_derank"], &g_vars.misc_auto_derank);
		get_val(settings["misc"]["notifications_hurt"], &g_vars.vis_misc_hurt_notifications);
		get_val(settings["misc"]["rank_reveal"], &g_vars.misc_rank_reveal);
		get_val(settings["misc"]["silent_walk"], &g_vars.misc_silent_walk);
		get_val(settings["misc"]["copy_bot"], &g_vars.misc_copy_bot);

		get_val(settings["walkbot"]["enabled"], &g_vars.misc_walkbot);
		get_val(settings["walkbot"]["walk"], &g_vars.misc_walkbot_walk);
		get_val(settings["walkbot"]["adaptive"], &g_vars.misc_walkbot_adaptive);
		get_val(settings["walkbot"]["add_key"], &g_vars.misc_walkbot_add_key);
		get_val(settings["walkbot"]["erase_key"], &g_vars.misc_walkbot_erase_key);

		get_val(settings["backtrack"]["enabled"], &g_vars.backtrack_enabled);
		get_val(settings["backtrack"]["skip_lc_breaker"], &g_vars.backtrack_lc_break);
		get_val(settings["backtrack"]["ticks"], &g_vars.backtrack_ticks);
		get_val(settings["backtrack"]["max_dist"], &g_vars.backtrack_dist);
		get_val(settings["backtrack"]["key"], &g_vars.backtrack_on_key);
		get_val(settings["backtrack"]["chams"], &g_vars.backtrack_chams);
		get_val(settings["backtrack"]["chams_styles"], &g_vars.backtrack_chams_style);
		get_val(settings["backtrack"]["chams_skip_lc_breaker"], &g_vars.backtrack_chams_lc_break);
		get_val(settings["backtrack"]["skeleton"], &g_vars.backtrack_skeleton);

		get_val(settings["resolver"]["predict_lby_update"], &g_vars.misc_predict_enemy_lby);
		get_val(settings["resolver"]["legit_aa_normal"], &g_vars.misc_resolver);
		get_val(settings["resolver"]["legit_aa_resolver_type"], &g_vars.misc_resolver_type);

		get_val(settings["chams"]["enabled"], &g_vars.chams_enabled);
		for (int i = 0; i < 3; i++)
		{
			get_val(settings["chams"][get_team(i)], &g_vars.chams_targets[i]);
		}
		get_val(settings["chams"]["ignorez"], &g_vars.chams_ignorez);
		get_val(settings["chams"]["wireframe"], &g_vars.chams_wireframe);
		get_val(settings["chams"]["flat"], &g_vars.chams_flat);
		get_val(settings["chams"]["glass"], &g_vars.chams_glass);
		get_val(settings["chams"]["metallic"], &g_vars.chams_reflective);

		get_val(settings["skinchanger"]["enabled"], &g_vars.skinchanger_enabled);

		for (int i = 0; i < menu_lists::weapon_list_names.size(); i++)
		{
			get_val(settings["skinchanger"]["skin_" + menu_lists::weapon_list.at(i).first + "_skin_id"], &skinchanger.info[menu_lists::weapon_list.at(i).second].skin_id);
			get_val(settings["skinchanger"]["skin_" + menu_lists::weapon_list.at(i).first + "_wear"], &skinchanger.info[menu_lists::weapon_list.at(i).second].wear);
			get_val(settings["skinchanger"]["skin_" + menu_lists::weapon_list.at(i).first + "_seed"], &skinchanger.info[menu_lists::weapon_list.at(i).second].seed);
		}

		for (int i = 0; i < 3; i++)
		{
			for (int v = 0; v < 2; v++)
			{
				//prevents logging localplayer occluded, which never happens
				if (i == 2 && v == 1)
					continue;

				get_val(settings["color"]["esp_" + get_team(i) + "_" + get_visibility(v)], &g_vars.color_esp[i][v]);
			}
		}

		get_val(settings["color"]["esp_c4"], &g_vars.color_esp_c4);

		for (int i = 0; i < 3; i++)
		{
			for (int v = 0; v < 2; v++)
			{
				//prevents logging localplayer occluded, which never happens
				if (i == 2 && v == 1)
					continue;

				get_val(settings["color"]["chams_" + get_team(i) + "_" + get_visibility(v)], &g_vars.color_chams[i][v]);
			}
		}

		for (int i = 0; i < 3; i++)
		{
			for (int v = 0; v < 2; v++)
			{
				//prevents logging localplayer occluded, which never happens
				if (i == 2 && v == 1)
					continue;

				get_val(settings["color"]["chams_backtrack_" + get_team(i) + "_" + get_visibility(v)], &g_vars.color_chams_backtrack[i][v]);
			}
		}

		for (int i = 0; i < 3; i++)
		{
			get_val(settings["color"]["footstep_beams_" + get_team(i)], &g_vars.color_footstep_beams[i]);
		}

		get_val(settings["color"]["hitmarker"], &g_vars.color_hitmarker);
		get_val(settings["color"]["hitmarker_text"], &g_vars.color_hitmarker_text);

		for (int i = 0; i < 3; i++)
		{
			get_val(settings["color"]["bullet_beam_" + get_team(i)], &g_vars.color_bullet_beams[i]);
		}

		for (int i = 0; i < 2; i++)
		{
			for (int v = 0; v < 2; v++)
			{

				get_val(settings["color"]["glow_" + get_team(i) + "_" + get_visibility(v)], &g_vars.color_glow[i][v]);
			}
		}

		get_val(settings["color"]["glow_weapons"], &g_vars.color_glow_others_weapons);
		get_val(settings["color"]["glow_c4"], &g_vars.color_glow_c4);

		get_val(settings["color"]["beam_smoke"], &g_vars.color_beam_smoke);
		get_val(settings["color"]["beam_molotov"], &g_vars.color_beam_molotov);
		get_val(settings["color"]["beam_decoy"], &g_vars.color_beam_decoy);

		get_val(settings["color"]["aimbot_target_text"], &g_vars.color_aimbot_target_text);

		get_val(settings["color"]["hurt_hitboxes"], &g_vars.color_esp_hurt_hitboxes);

		get_val(settings["color"]["spread_crosshair"], &g_vars.color_spread_crosshair);

		get_val(settings["color"]["offscreen_esp"], &g_vars.color_offscreen_esp);

		for (int i = 0; i < 3; i++)
		{
			get_val(settings["color"]["bullet_impact_" + get_team(i)], &g_vars.color_bullet_impacts[i]);
		}

		get_val(settings["color"]["taser_range"], &g_vars.color_taser_range);

		get_val(settings["color"]["hud_features_text"], &g_vars.color_hud_feature_text);
		get_val(settings["color"]["hud_features_box"], &g_vars.color_hud_feature_box);

		get_val(settings["color"]["origin_box"], &g_vars.color_origin_box);

		get_val(settings["color"]["walkbot_next"], &g_vars.color_walkbot_next);
		get_val(settings["color"]["walkbot_etc"], &g_vars.color_walkbot_etc);

		get_val(settings["color"]["fov_ring"], &g_vars.color_fov_ring);

		for (int i = 0; i < 3; i++)
		{
			get_val(settings["color"]["viewangles_" + get_team(i)], &g_vars.color_viewangles[i]);
		}

		get_val(settings["color"]["autowall_crosshair"], &g_vars.color_autowall_crosshair);

		get_val(settings["color"]["planting_bar"], &g_vars.color_planting_bar);
		get_val(settings["color"]["defusing_bar"], &g_vars.color_defusing_bar);
		get_val(settings["color"]["dz_item_esp"], &g_vars.color_dangerzone_items);

		for (int i = 0; i < 3; i++)
		{
			get_val(settings["color"]["backtrack_skeleton" + get_team(i)], &g_vars.color_backtrack_skeleton[i]);
		}
	}
	
	void save_cfg(std::string file_name)
	{
		Json::Value settings = get_settings();

		std::string str_json = settings.toStyledString();

		std::string final_path = "C:\\VER$ACE\\configs\\" + file_name;
		FILE* file = fopen(final_path.c_str(), "w+");
		if (file)
		{
			fwrite(str_json.c_str(), 1, str_json.length(), file);
			fclose(file);
		}
	}
	
	void load_cfg(std::string file_name)
	{
		std::string final_path = "C:\\VER$ACE\\configs\\" + file_name;

		if(!std::filesystem::exists(final_path))
		{
			push_notifications::Push("file: " + final_path + " does not exist.");
			return;
		}
		
		FILE* infile = fopen(final_path.c_str(), "r");

		if (!infile)
		{
			save_cfg(final_path);
			return;
		}

		fseek(infile, 0, SEEK_END);
		long filesize = ftell(infile);
		char* buf = new char[filesize + 1];
		fseek(infile, 0, SEEK_SET);
		fread(buf, 1, filesize, infile);
		fclose(infile);

		buf[filesize] = '\0';
		std::stringstream ss;
		ss.str(buf);
		delete[] buf;

		Json::Value settings;
		ss >> settings;

		load_cfg_internal(settings);
	}

	void load_from_web(std::string tag)
	{
		c_http_client http_client = c_http_client();
		std::string data = "tag=" + tag;
		std::string response_string = http_client.send_post(L"web_based_configs/get_config.php", data);

		//maybe not a config file
		std::stringstream ss;
		ss.str(response_string.c_str());

		Json::Value reply;
		ss >> reply;
		if (!reply["status"].isNull() && reply["status"] == "failed")
		{
			std::string msg = "failed to get config, reason: " + reply["detail"].asString();
			MessageBoxA(NULL, msg.c_str(), "failed.", 0);
			return;
		}

		load_cfg_internal(reply);
	}

	void save_to_web(std::string tag)
	{
		Json::Value settings = get_settings();
		Json::StyledWriter styled_writer;
		std::string str_json = styled_writer.write(settings);

		std::string b64 = Utils::base64_encode(reinterpret_cast<const unsigned char*>(str_json.c_str()), str_json.length());

		c_http_client http_client = c_http_client();
		std::string data = "username=" + info_lp::username + "&tag=" + tag + "&cfg=" + b64;

		std::string response_string = http_client.send_post(L"web_based_configs/save_config.php", data);

		std::stringstream ss;
		ss.str(response_string);

		Json::Value reply;
		ss >> reply;

		if (reply["status"] == "failed")
		{
			std::string msg = "failed upload, reason: " + reply["detail"].asString();
			MessageBoxA(NULL, msg.c_str(), "failed.", 0);
		}
		else
		{
			std::string msg = "upload succeeded, tag: " + tag;
			MessageBoxA(NULL, msg.c_str(), "success", 0);
		}
	}

	std::vector<std::string> split(std::string stringToBeSplitted, std::string delimeter)
	{
		std::vector<std::string> splittedString;
		int startIndex = 0;
		int  endIndex = 0;
		int split_idx = 0;
		while ((endIndex = stringToBeSplitted.find(delimeter, startIndex)) < stringToBeSplitted.size())
		{
			std::string val = stringToBeSplitted.substr(startIndex, endIndex - startIndex);
			splittedString.push_back(val);
			startIndex = endIndex + delimeter.size();
		}
		if (startIndex < stringToBeSplitted.size())
		{
			std::string val = stringToBeSplitted.substr(startIndex);
			splittedString.push_back(val);
		}
		return splittedString;
	}

	std::vector<std::string> get_top_10()
	{
		c_http_client http_client = c_http_client();
		std::string response_string = http_client.send_get(L"web_based_configs/top.php");

		auto split_vals = split(response_string, "\n");

		if (split_vals.back().empty())
			split_vals.erase(split_vals.end());

		return split_vals;
	}

	std::vector<std::string> get_local_configs()
	{
		std::vector<std::string> files = {};
		auto path = std::filesystem::path("C:\\VER$ACE\\configs\\");

		if (!std::filesystem::exists(path))
			return {};
		
		for(const auto& file : std::filesystem::directory_iterator("C:\\VER$ACE\\configs\\"))
		{
			if (file.is_directory())
				continue;

			auto file_as_path = std::filesystem::path(file);
			files.emplace_back(file_as_path.filename().generic_string());
		}

		return files;
	}
}
