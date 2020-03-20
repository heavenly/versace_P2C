#include "Menu.hpp"
#define NOMINMAX
#include <Windows.h>
#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "options.hpp"
#include "ui.hpp"
#include "anticheat.h"
#include "json_configs.h"
#include "skinchanger.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/directx9/imgui_impl_dx9.h"

#include "04b03.h"
#include "lists.h"
#include "etc_funcs.h"

#include "tas_bot.h"
#include "kaching.h"
#include "resolver.h"
#include "notification.h"
#include "local_info.h"
#include "friends_list.h"
#include "valve_sdk/interfaces/ICvar.hpp"
#include "valve_sdk/item_definitions.hpp"
#include "valve_sdk/misc/Convar.hpp"
#include "js_functions.hpp"
#include "js_loader.h"

#pragma comment(lib, "winmm.lib")

#define RGBA_TO_FLOAT(r,g,b,a) (float)r/255.0f, (float)g/255.0f, (float)b/255.0f, (float)a/255.0f

static ConVar* cl_mouseenable = nullptr;

constexpr auto size_x = 400;
constexpr auto size_y = 300;

void Menu::Initialize()
{
	_visible = true;

	cl_mouseenable = g_CVar->FindVar("cl_mouseenable");

	create_style();
}

void Menu::Shutdown()
{
	ImGui_ImplDX9_Shutdown();
	cl_mouseenable->SetValue(true);
}

void Menu::OnDeviceLost()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
	ImGui_ImplDX9_CreateDeviceObjects();
}

void aimbot_tab()
{
	ImGui::Checkbox("aimbot", &g_vars.aimbot_enabled);
	if (g_vars.aimbot_enabled)
	{
		ImGui::Selectable("enemies ## aimbot", &g_vars.aimbot_targets[1]);
		ImGui::Selectable("teammates ## aimbot", &g_vars.aimbot_targets[0]);
		ImGui::Hotkey("aim key", &g_vars.aimbot_on_key);

		if (!g_vars.aimbot_autowall)
			ImGui::Checkbox("vischeck", &g_vars.aimbot_vis_check);

		ImGui::Checkbox("silent", &g_vars.aimbot_silent_aim);

		if (!g_vars.aimbot_vis_check)
			ImGui::Checkbox("autowall", &g_vars.aimbot_autowall);

		if (g_vars.aimbot_autowall)
		{
			if (!g_vars.aimbot_get_best_hitbox)
			{
				ImGui::Checkbox("hitscan", &g_vars.hitscan_enabled);
			}
			ImGui::SliderFloat("autowall dmg", &g_vars.aimbot_autowall_dmg, 0, 100);
			ImGui::Checkbox("autoshoot", &g_vars.aimbot_autoshoot_enabled);
			ImGui::Checkbox("autoscope", &g_vars.aimbot_autoscope_enabled);
			ImGui::Checkbox("autostop", &g_vars.aimbot_autostop);
		}
		if (!g_vars.hitscan_enabled)
			ImGui::Checkbox("get best hitbox", &g_vars.aimbot_get_best_hitbox);

		if (g_vars.aimbot_get_best_hitbox)
			ImGui::SliderInt("hitbox scan time delay", &g_vars.aimbot_get_best_hitbox_scan_time, 0, 1000);

		if (g_vars.aimbot_vis_check)
		{
			//ImGui::Checkbox("reaction delay", &g_vars.aimbot_reaction);

			//if (g_vars.aimbot_reaction)
				//ImGui::SliderInt("reaction time (ms)", &g_vars.aimbot_reaction_time, 0, 10000);
		}

		ImGui::Checkbox("target change cooldown", &g_vars.aimbot_target_cooldown);

		if (g_vars.aimbot_target_cooldown)
			ImGui::SliderFloat("cooldown time (ms)", &g_vars.aimbot_target_time, 0, 10000);

		ImGui::Checkbox("on attack", &g_vars.aimbot_on_attack);
		ImGui::SliderInt("aim after x shots", &g_vars.aimbot_aim_after_shots, 0, 29);
		ImGui::Checkbox("nonsticky", &g_vars.aimbot_non_sticky);
		ImGui::Checkbox("aim chance", &g_vars.aimbot_aim_chance);

		if (g_vars.aimbot_aim_chance)
			ImGui::SliderInt("aim chance %", &g_vars.aimbot_aim_chance_percent, 0, 100);

		ImGui::Checkbox("avoid jumping players", &g_vars.aimbot_avoid_jumping);
		ImGui::Checkbox("smoke check", &g_vars.aimbot_smoke_check);
		ImGui::Checkbox("flash check", &g_vars.aimbot_flash_check);
		ImGui::SliderFloat("fov", &g_vars.aimbot_fov, 0, 180);
		ImGui::SliderFloat("smoothing", &g_vars.aimbot_smooth, 0, 1);

		if (!g_vars.aimbot_get_best_hitbox)
			ImGui::Combo("hitbox", &g_vars.aimbot_hitbox, menu_lists::hitboxes);

		ImGui::Checkbox("speed limit", &g_vars.aimbot_speed_limit);

		if (g_vars.aimbot_speed_limit)
			ImGui::SliderInt("speed", &g_vars.aimbot_speed, 0, 300);

		//if (g_vars.backtrack_enabled)
			//ImGui::Checkbox("aim at backtrack ticks", &g_vars.aimbot_get_best_backtrack);

	}

	ImGui::Checkbox("rcs", &g_vars.misc_rcs);

	if (g_vars.misc_rcs)
		ImGui::SliderFloat("amount", &g_vars.misc_rcs_amount, 0, 2);

	ImGui::Checkbox("backtrack", &g_vars.backtrack_enabled);
	if (g_vars.backtrack_enabled)
	{
		ImGui::Hotkey("backtrack key", &g_vars.backtrack_on_key);
		ImGui::SliderInt("ticks", &g_vars.backtrack_ticks, 0, 30);
		ImGui::Checkbox("skip lc break dist ## bt_controls", &g_vars.backtrack_lc_break);
		ImGui::SliderFloat("max backtrack dist", &g_vars.backtrack_dist, 0, 128);
	}
}

void triggerbot_tab()
{
	ImGui::Checkbox("triggerbot", &g_vars.triggerbot_enabled);
	if (g_vars.triggerbot_enabled)
	{
		ImGui::Selectable("enemies ## trigger", &g_vars.triggerbot_targets[1]);
		ImGui::Selectable("teammates ## trigger", &g_vars.triggerbot_targets[0]);
		ImGui::Hotkey("trigger key", &g_vars.triggerbot_key);
		ImGui::Checkbox("shoot at backtrack", &g_vars.triggerbot_backtrack);
		ImGui::SliderInt("trigger delay (ms)", &g_vars.triggerbot_delay, 0, 10000);
		for (size_t i = 0; i < menu_lists::hitboxes_shortened.size(); i++)
		{
			ImGui::Selectable(menu_lists::hitboxes_shortened.at(i).c_str(), &g_vars.triggerbot_hitboxes[i]);
		}
	}
}


void vis_misc_tab()
{
	ImGui::Checkbox("spectator list", &g_vars.misc_spec_list);
	ImGui::Checkbox("aimbot target", &g_vars.esp_aimbot_target);
	etc_functions::ColorPicker("aimbot_target", &g_vars.color_aimbot_target_text);
	ImGui::Checkbox("footstep beams", &g_vars.vis_misc_footstep_beams);
	ImGui::Checkbox("bullet beams", &g_vars.vis_misc_bullet_beams);
	ImGui::Combo("beam type", &g_vars.vis_misc_bullet_beams_type, menu_lists::bullet_beam_types);
	if (g_vars.vis_misc_bullet_beams)
	{
		ImGui::SliderFloat("bullet beam time", &g_vars.vis_misc_bullet_beams_lifetime, 1, 10);
		if (g_vars.vis_misc_bullet_beams_type == 0)
			ImGui::SliderFloat("bullet beam size", &g_vars.vis_misc_bullet_beams_size, 1, 20);
		ImGui::Selectable("team bullet beams", &g_vars.vis_misc_bullet_beams_targets[0]);
		ImGui::Selectable("enemy bullet beams", &g_vars.vis_misc_bullet_beams_targets[1]);
		ImGui::Selectable("local bullet beams", &g_vars.vis_misc_bullet_beams_targets[2]);
	}
	ImGui::Checkbox("bullet impact boxes", &g_vars.vis_misc_bullet_impacts);
	if (g_vars.vis_misc_bullet_impacts)
	{
		ImGui::SliderFloat("bullet impact time", &g_vars.vis_misc_bullet_impacts_lifetime, 1, 10);
		ImGui::SliderInt("bullet impact size", &g_vars.vis_misc_bullet_impacts_size, 1, 20);
		ImGui::Selectable("team bullet impacts", &g_vars.vis_misc_bullet_impacts_targets[0]);
		ImGui::Selectable("enemy bullet impacts", &g_vars.vis_misc_bullet_impacts_targets[1]);
		ImGui::Selectable("local bullet impacts", &g_vars.vis_misc_bullet_impacts_targets[2]);
	}
	ImGui::Checkbox("grenade circles", &g_vars.vis_misc_grenade_circle);

	ImGui::Checkbox("hitmarkers", &g_vars.vis_misc_hitmarkers);
	if (g_vars.vis_misc_hitmarkers)
	{
		ImGui::Combo("hitmarker sound", &g_vars.vis_misc_hitmarkers_style, menu_lists::hitmarker_sounds);
		ImGui::Checkbox("show backtracked tick", &g_vars.vis_misc_hitmarkers_bt_tick);
		ImGui::Checkbox("show hitbox", &g_vars.vis_misc_hitmarkers_hitbox);
	}
	ImGui::Checkbox("no smoke", &g_vars.vis_misc_no_smoke);
	ImGui::Checkbox("no flash", &g_vars.misc_no_flash);
	ImGui::Checkbox("spread crosshair", &g_vars.vis_misc_spread_crosshair);
	etc_functions::ColorPicker("spread_crosshair", &g_vars.color_spread_crosshair);

	ImGui::Checkbox("draw taser range", &g_vars.vis_misc_taser_range);
	etc_functions::ColorPicker("taser_range", &g_vars.color_taser_range);
	ImGui::SliderInt("override fov", &g_vars.vis_misc_override_fov, 0, 180);
	//ImGui::SliderInt("override viewmodel fov", &g_vars.vis_misc_override_viewmodel_fov, 0, 180);
	//ImGui::Checkbox("draw grenade path", &g_vars.vis_misc_draw_grenade_path);
	ImGui::Checkbox("hud features", &g_vars.vis_misc_hud_features);
	etc_functions::ColorPicker("hud_text", &g_vars.color_hud_feature_text);
	if (g_vars.vis_misc_hud_features)
	{
		ImGui::Checkbox("hud features bg box", &g_vars.vis_misc_hud_features_box);
		etc_functions::ColorPicker("hud_box", &g_vars.color_hud_feature_box);
	}
	ImGui::Checkbox("fov ring", &g_vars.vis_misc_fov_ring);
	etc_functions::ColorPicker("fov_ring", &g_vars.color_fov_ring);

	ImGui::Checkbox("autowall crosshair", &g_vars.vis_misc_autowall_crosshair);
	etc_functions::ColorPicker("autowall_cross", &g_vars.color_autowall_crosshair);
	ImGui::Checkbox("nightmode", &g_vars.misc_nightmode);
	if (g_vars.misc_nightmode)
	{
		ImGui::SliderFloat("nightmode normal", &g_vars.misc_nightmode_normal_val, 0, 1.f);
		ImGui::SliderFloat("nightmode props", &g_vars.misc_nightmode_staticprop_val, 0, 1.f);
	}
	ImGui::Checkbox("asus", &g_vars.misc_asus);
	if (g_vars.misc_asus)
	{
		ImGui::SliderFloat("asus normal", &g_vars.misc_asus_normal_val, 0, 1.f);
		ImGui::SliderFloat("asus props", &g_vars.misc_asus_staticprop_val, 0, 1.f);
	}
	ImGui::Checkbox("damage notifications", &g_vars.vis_misc_hurt_notifications);
	ImGui::Checkbox("list purchases", &g_vars.vis_misc_log_purchases);
	ImGui::Checkbox("dangerzone item esp", &g_vars.esp_draw_dangerzone_items);
	etc_functions::ColorPicker("dz_item_esp", &g_vars.color_dangerzone_items);
}

void chams_tab()
{
	ImGui::Checkbox("enable ## chams", &g_vars.chams_enabled);
	if (g_vars.chams_enabled)
	{
		ImGui::Selectable("teammates", &g_vars.chams_targets[0]);
		ImGui::Selectable("enemies", &g_vars.chams_targets[1]);
		ImGui::Selectable("local", &g_vars.chams_targets[2]);
		ImGui::Checkbox("ignorez", &g_vars.chams_ignorez);
		ImGui::Checkbox("wireframe", &g_vars.chams_wireframe);
		ImGui::Checkbox("pulse", &g_vars.chams_pulse);
		ImGui::Checkbox("flat", &g_vars.chams_flat);
		ImGui::Checkbox("glass", &g_vars.chams_glass);
		ImGui::Checkbox("metallic", &g_vars.chams_reflective);
		ImGui::Checkbox("backtrack chams", &g_vars.backtrack_chams);
		if (g_vars.backtrack_chams)
		{
			ImGui::Combo("bt chams style", &g_vars.backtrack_chams_style, menu_lists::backtrack_chams_styles);
			ImGui::Checkbox("skip lc break dist", &g_vars.backtrack_chams_lc_break);
		}
	}
}

void esp_tab()
{
	ImGui::Checkbox("enable ## esp", &g_vars.esp_enabled);
	if (g_vars.esp_enabled)
	{
		ImGui::Hotkey("esp hold key", &g_vars.esp_on_key);
		ImGui::SliderInt("esp draw dist", &g_vars.esp_drawing_dist, 1, 20000);
		ImGui::Selectable("teammates", &g_vars.esp_targets[0]);
		ImGui::Selectable("enemies", &g_vars.esp_targets[1]);
		ImGui::Selectable("local", &g_vars.esp_targets[2]);
		ImGui::Checkbox("box", &g_vars.esp_player_boxes);
		ImGui::Checkbox("name", &g_vars.esp_player_names);
		ImGui::Checkbox("armor", &g_vars.esp_player_armour);
		if (g_vars.esp_player_armour)
		{
			ImGui::SliderInt("armor bar thickness", &g_vars.esp_player_armour_thick, 2, 5);
			ImGui::Checkbox("fractional bar ## armor", &g_vars.esp_player_armour_fractional);
		}
		ImGui::Checkbox("health", &g_vars.esp_player_health);
		if (g_vars.esp_player_health)
		{
			ImGui::SliderInt("health bar thickness", &g_vars.esp_player_health_thick, 2, 5);
			ImGui::Checkbox("fractional bar ## health", &g_vars.esp_player_health_fractional);
		}
		ImGui::Checkbox("weapon", &g_vars.esp_player_weapons);
		ImGui::Checkbox("ammo bar", &g_vars.esp_player_ammo_bar);
		etc_functions::ColorPicker("ammo_bar", &g_vars.color_ammo_bar);
		ImGui::Checkbox("money", &g_vars.esp_player_money);
		ImGui::Checkbox("vulnerabilities", &g_vars.esp_player_vulnerabilities);

		ImGui::Checkbox("viewangles", &g_vars.esp_player_viewangles);

		if (g_vars.esp_player_viewangles)
			ImGui::SliderInt("viewangles length", &g_vars.esp_viewangles_length, 1, 8192);

		ImGui::Checkbox("distance", &g_vars.esp_distance);

		if (g_vars.esp_distance)
			ImGui::Combo("distance display style", &g_vars.esp_distance_style, menu_lists::distance_styles);

		ImGui::Checkbox("bomb carrier", &g_vars.esp_player_c4);
		ImGui::Checkbox("defuse kit", &g_vars.esp_player_defuser);

		ImGui::Checkbox("far esp", &g_vars.esp_farther);
		ImGui::Checkbox("snaplines", &g_vars.esp_player_snaplines);
		ImGui::Checkbox("offscreen esp", &g_vars.esp_offscreen);
		etc_functions::ColorPicker("offscreen_esp", &g_vars.color_offscreen_esp);
		if (g_vars.esp_offscreen)
		{
			ImGui::SliderInt("triangle width", &g_vars.esp_offscreen_width, 1, 50);
			ImGui::SliderInt("triangle height", &g_vars.esp_offscreen_height, 1, 50);
		}

		ImGui::Checkbox("hurt hitboxes", &g_vars.esp_hurt_hitboxes);
		etc_functions::ColorPicker("hurt_hitboxes", &g_vars.color_esp_hurt_hitboxes); //this is called from server.dll so we can't change it ourself
		if (g_vars.esp_hurt_hitboxes)
			ImGui::SliderFloat("hurt hitboxes duration", &g_vars.esp_hurt_hitboxes_time, 1, 10);

		ImGui::Checkbox("origin box", &g_vars.esp_origin_box);
		etc_functions::ColorPicker("origin box", &g_vars.color_origin_box);
		if (g_vars.esp_origin_box)
		{
			ImGui::SliderFloat("origin box size", &g_vars.esp_origin_box_size, 1, 30);
			ImGui::Combo("origin box style", &g_vars.esp_origin_box_style, menu_lists::origin_box_styles);
		}

		/*ImGui::Checkbox("planting bar", &g_vars.esp_player_planting_bar);
		etc_functions::ColorPicker("planting bar", &g_vars.color_planting_bar);
		ImGui::Checkbox("defusing bar", &g_vars.esp_player_defusing_bar);
		etc_functions::ColorPicker("defusing bar", &g_vars.color_defusing_bar);*/

		ImGui::Checkbox("skeleton", &g_vars.esp_player_skeleton);
		if (g_vars.backtrack_enabled)
			ImGui::Checkbox("backtrack skeleton", &g_vars.backtrack_skeleton);

		ImGui::Checkbox("vs user", &g_vars.esp_draw_vs_user);
		//ImGui::Checkbox("planted c4", &g_vars.esp_planted_c4);
	}
}

void glow_tab()
{
	ImGui::Checkbox("enable ## glow", &g_vars.glow_enabled);
	if (g_vars.glow_enabled)
	{
		ImGui::Hotkey("glow hold key", &g_vars.glow_on_key);

		ImGui::Checkbox("players ## glow", &g_vars.glow_players);
		ImGui::Checkbox("others ## glow", &g_vars.glow_others);
		if (g_vars.glow_others)
			ImGui::Combo("others glow style", &g_vars.glow_others_style, menu_lists::glow_styles);

	}
}

void colors_tab()
{
	if (g_vars.chams_enabled)
	{
		if (g_vars.chams_targets[0])
		{
			etc_functions::color_edit_4("chams ally visible", &g_vars.color_chams[0][0], true);
			if (g_vars.chams_ignorez)
				etc_functions::color_edit_4("chams ally occluded", &g_vars.color_chams[0][1], true);

			etc_functions::color_edit_4("bt chams ally visible", &g_vars.color_chams_backtrack[0][0], true);
			if (g_vars.chams_ignorez)
				etc_functions::color_edit_4("bt chams ally occluded", &g_vars.color_chams_backtrack[0][1], true);

		}
		if (g_vars.chams_targets[1])
		{
			etc_functions::color_edit_4("chams enemy visible", &g_vars.color_chams[1][0], true);
			if (g_vars.chams_ignorez)
				etc_functions::color_edit_4("chams enemy occluded", &g_vars.color_chams[1][1], true);

			etc_functions::color_edit_4("bt chams enemy visible", &g_vars.color_chams_backtrack[1][0], true);
			if (g_vars.chams_ignorez)
				etc_functions::color_edit_4("bt chams enemy occluded", &g_vars.color_chams_backtrack[1][1], true);
		}
		if (g_vars.chams_targets[2])
			etc_functions::color_edit_4("chams local", &g_vars.color_chams[2][0], true);
		ImGui::Spacing();
	}

	if (g_vars.esp_enabled)
	{
		if (g_vars.esp_targets[0])
		{
			etc_functions::color_edit_4("esp ally visible", &g_vars.color_esp[0][0], true);
			etc_functions::color_edit_4("esp ally occluded", &g_vars.color_esp[0][1], true);

			if (g_vars.vis_misc_footstep_beams)
				etc_functions::color_edit_4("footsteps ally", &g_vars.color_footstep_beams[0], true);
			if(g_vars.backtrack_skeleton)
				etc_functions::color_edit_4("backtrack skeleton ally", &g_vars.color_backtrack_skeleton[0], true);
		}
		if (g_vars.esp_targets[1])
		{
			etc_functions::color_edit_4("esp enemy visible", &g_vars.color_esp[1][0], true);
			etc_functions::color_edit_4("esp enemy occluded", &g_vars.color_esp[1][1], true);

			if (g_vars.vis_misc_footstep_beams)
				etc_functions::color_edit_4("footsteps enemy", &g_vars.color_footstep_beams[1], true);
			if (g_vars.backtrack_skeleton)
				etc_functions::color_edit_4("backtrack skeleton ally", &g_vars.color_backtrack_skeleton[1], true);
		}
		if (g_vars.esp_targets[2])	
		{
			etc_functions::color_edit_4("esp local", &g_vars.color_esp[2][0], true);

			if (g_vars.vis_misc_footstep_beams)
				etc_functions::color_edit_4("footsteps local", &g_vars.color_footstep_beams[2], true);
			if (g_vars.backtrack_skeleton)
				etc_functions::color_edit_4("backtrack skeleton ally", &g_vars.color_backtrack_skeleton[2], true);
		}
		ImGui::Spacing();
		if (g_vars.vis_misc_hitmarkers)
		{
			etc_functions::color_edit_4("hitmarker", &g_vars.color_hitmarker);
			etc_functions::color_edit_4("hitmarker text", &g_vars.color_hitmarker_text);
		}
		if (g_vars.vis_misc_grenade_circle)
		{
			etc_functions::color_edit_4("decoy", &g_vars.color_beam_decoy, true);
			etc_functions::color_edit_4("molotov", &g_vars.color_beam_molotov, true);
			etc_functions::color_edit_4("smoke", &g_vars.color_beam_smoke, true);
		}
		if (g_vars.misc_walkbot)
		{
			etc_functions::color_edit_4("walkbot_next", &g_vars.color_walkbot_next, true);
			etc_functions::color_edit_4("walkbot_etc", &g_vars.color_walkbot_etc, true);
		}
		ImGui::Spacing();
		if (g_vars.vis_misc_bullet_beams)
		{

			if (g_vars.vis_misc_bullet_beams_targets[0])
				etc_functions::color_edit_4("team beams", &g_vars.color_bullet_beams[0], false);

			if (g_vars.vis_misc_bullet_beams_targets[1])
				etc_functions::color_edit_4("enemy beams", &g_vars.color_bullet_beams[1], false);

			if (g_vars.vis_misc_bullet_beams_targets[2])
				etc_functions::color_edit_4("local beams", &g_vars.color_bullet_beams[2], false);
			ImGui::Spacing();

		}
		if (g_vars.vis_misc_bullet_impacts)
		{
			if (g_vars.vis_misc_bullet_impacts_targets[0])
				etc_functions::color_edit_4("team impacts", &g_vars.color_bullet_impacts[0], false);

			if (g_vars.vis_misc_bullet_impacts_targets[1])
				etc_functions::color_edit_4("enemy impacts", &g_vars.color_bullet_impacts[1], false);

			if (g_vars.vis_misc_bullet_impacts_targets[2])
				etc_functions::color_edit_4("local impacts", &g_vars.color_bullet_impacts[2], false);

			ImGui::Spacing();
		}
		ImGui::Spacing();
		if (g_vars.esp_player_viewangles)
		{

			if (g_vars.esp_targets[0])
				etc_functions::color_edit_4("team viewangles", &g_vars.color_viewangles[0], false);

			if (g_vars.esp_targets[1])
				etc_functions::color_edit_4("enemy viewangles", &g_vars.color_viewangles[1], false);

			if (g_vars.esp_targets[2])
				etc_functions::color_edit_4("local viewangles", &g_vars.color_viewangles[2], false);

			ImGui::Spacing();

		}
		ImGui::Spacing();
	}

	/*if (g_vars.glow_enabled)
	{
		etc_functions::color_edit_4("glow ally visible", &g_vars.color_glow[0][0], true);
		etc_functions::color_edit_4("glow ally occluded", &g_vars.color_glow[0][1], true);
		etc_functions::color_edit_4("glow enemy visible", &g_vars.color_glow[1][0], true);
		etc_functions::color_edit_4("glow enemy occluded", &g_vars.color_glow[1][1], true);

		if (g_vars.glow_others)
		{
			etc_functions::color_edit_4("glow weapons", &g_vars.color_glow_others_weapons, true);
			etc_functions::color_edit_4("glow c4", &g_vars.color_glow_c4, true);
		}
		ImGui::Spacing();
	}*/
}

void visual_tab()
{
	static int visual_tab_num = 0;
	constexpr auto size_x_tab = (size_x / 4) - 15;
	if (ImGui::Button("esp", ImVec2(size_x_tab, 30)))
		visual_tab_num = 0;

	ImGui::SameLine();
	if (ImGui::Button("chams", ImVec2(size_x_tab, 30)))
		visual_tab_num = 1;

	//ImGui::SameLine();
	//if (ImGui::Button("glow", ImVec2(size_x_tab, 30)))
		//visual_tab_num = 2;

	ImGui::SameLine();

	if (ImGui::Button("misc ## vis", ImVec2(size_x_tab, 30)))
		visual_tab_num = 3;

	ImGui::SameLine();
	if (ImGui::Button("colors", ImVec2(size_x_tab, 30)))
		visual_tab_num = 4;

	ImGui::Separator();
	switch (visual_tab_num)
	{
	case 0:esp_tab(); break;
	case 1:chams_tab(); break;
	//case 2:glow_tab(); break;
	case 3:vis_misc_tab(); break;
	case 4:colors_tab(); break;
	}
}

void players_tab()
{
	if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected() && g_LocalPlayer)
	{
		for (size_t i = 1; i < g_EngineClient->GetMaxClients(); ++i)
		{
			C_BasePlayer *entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
			if (!entity)
				continue;

			player_info_t ent_info;
			g_EngineClient->get_player_info(i, &ent_info);

			if (ent_info.fakeplayer || ent_info.ishltv)
				continue;

			if (ImGui::CollapsingHeader(ent_info.szName))
			{
				std::string choked_ticks = "choked ticks -> " + std::to_string(anticheat.players[i].choked_ticks);
				std::string angle_delta = "angle delta -> " + std::to_string(anticheat.players[i].angle_delta);
				std::string cur_simtime = "current simtime -> " + std::to_string(entity->GetSimulationTime());
				std::string best_bt_time = "best rage bt time -> " + std::to_string(anticheat.best_time[i]);
				std::string resolver_misses = "resolver misses -> " + std::to_string(resolver.player_res_info[i].shots_missed);
				std::string resolver_info = "moving lby -> " + std::to_string(resolver.player_res_info[i].moving_lby) + " current lby -> " + std::to_string(entity->m_flLowerBodyYawTarget());

				ImGui::Text(choked_ticks.c_str());
				ImGui::Text(angle_delta.c_str());
				ImGui::Text(cur_simtime.c_str());
				ImGui::Text(best_bt_time.c_str());
				ImGui::Text(resolver_misses.c_str());
				ImGui::Text(resolver_info.c_str());

				if (ImGui::Button("steal name"))
				{
					std::string name(ent_info.szName);
					name += " ";
					Utils::SetName(name.c_str());
					push_notifications::Push("stole name: " + name);
				}

				if (ImGui::Button("add to friends"))
				{
					modified_friends_list::friends.emplace_back(ent_info.steamID64);
					push_notifications::Push("added steam_id-" + std::to_string(ent_info.steamID64) + " to modified friends list");
				}
			}
		}
	}
	else
		ImGui::Text("you must be in game for this to work!");
}


void config_tab()
{
	//buffer overflow by this im pretty sure
	static char config_name[128] = "";
	ImGui::InputText("config name", config_name, IM_ARRAYSIZE(config_name));

	if (ImGui::Button("save config"))
	{
		Configs::save_cfg(config_name);
		push_notifications::Push("saved config " + std::string(config_name));
	}

	if (ImGui::Button("load config"))
	{
		Configs::load_cfg(config_name);
		push_notifications::Push("loaded config " + std::string(config_name));
	}

	static bool got_local_configs = false;
	static std::vector<std::string> local_configs_list;
	if (!got_local_configs)
	{
		local_configs_list = Configs::get_local_configs();
		got_local_configs = true;
	}

	ImGui::Text("local configs: ");
	ImGui::BeginChild("local configs", ImVec2(350, 90), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
	if (local_configs_list.empty())
		ImGui::Text("You have no saved configs!");
	else
	{
		for(auto local_cfg_name : local_configs_list)
		{
			ImGui::Text(local_cfg_name.c_str());
		}
	}
	ImGui::EndChild();

	if (ImGui::Button("refresh local configs"))
	{
		local_configs_list = Configs::get_local_configs();
	}
	
	ImGui::Separator();
	static char web_config_tag[16] = "";
	ImGui::InputText("web config name", web_config_tag, IM_ARRAYSIZE(web_config_tag));

	if (ImGui::Button("save config to web"))
	{
		Configs::save_to_web(web_config_tag);
		push_notifications::Push("saved web config " + std::string(web_config_tag));
	}

	if (ImGui::Button("load config from web"))
	{
		Configs::load_from_web(web_config_tag);
		push_notifications::Push("loaded web config " + std::string(web_config_tag));
	}

	//draw 10 most recent cfgs - doesnt work, fix later
	
	static bool got_10 = false;
	static std::vector<std::string> results_10;
	if (!got_10)
	{
		results_10 = Configs::get_top_10();
		got_10 = true;
	}

	if (ImGui::Button("refresh recent configs"))
	{
		results_10 = Configs::get_top_10();
	}


	ImGui::Text("recent web configs: ");
	if (results_10.empty())
		ImGui::Text("no recent web configs.");
	else
	{
		ImGui::BeginChild("recent web configs", ImVec2(350, 90), true);
		for (const auto& val : results_10)
		{
			std::stringstream ss;
			ss.str(val);

			Json::Value reply;
			ss >> reply;

			std::string result_txt = "user: " + reply["creator"].asString() + " tag: " + reply["tag"].asString();

			ImGui::Text(result_txt.c_str());
		}
		ImGui::EndChild();
	}

	static auto js_files = c_js_loader::refresh_scripts(js_funcs.get_js_files());
	
	if(ImGui::Button("refresh js files"))
	{
		js_files = c_js_loader::refresh_scripts(js_funcs.get_js_files());
	}
	
	ImGui::Text("js files: ");
	if(js_files.empty())
	{
		ImGui::Text("no js files in C:\\VER$ACE\\scripts");
	}
	else
	{
		for(auto& js_script : c_js_loader::scripts) //get straight from class so we can edit
		{
			ImGui::Text(js_script.name.c_str());
			ImGui::SameLine();
			std::string new_cb_name = "enabled ##" + js_script.name;
			ImGui::Checkbox(new_cb_name.c_str(), &js_script.is_enabled);
			ImGui::SameLine();
			std::string new_combo_name = "execution location ##" + js_script.name;
			ImGui::Combo(new_combo_name.c_str(), &js_script.execution_location, menu_lists::execution_loc_types);
		}
	}
	
}

void aa_tab()
{

	ImGui::Checkbox("antiaim ## cb", &g_vars.misc_legit_aa);
	if (g_vars.misc_legit_aa)
	{
		ImGui::SliderInt("antiaim angle", &g_vars.misc_legit_aa_ang, -180, 180);
		//ImGui::Checkbox("break lby", &g_vars.misc_legit_aa_break_lby);
		//if (g_vars.misc_legit_aa_break_lby)
			//ImGui::SliderInt("lby dist", &g_vars.misc_legit_aa_break_lby_dist, -180, 180);

		ImGui::Checkbox("pitch aa", &g_vars.misc_legit_aa_pitch);

		if (g_vars.misc_legit_aa_pitch)
			ImGui::SliderInt("pitch aa val", &g_vars.misc_legit_aa_pitch_val, -89, 89);
	}
	ImGui::Checkbox("fakelag", &g_vars.misc_legit_aa_fakelag);

	if (g_vars.misc_legit_aa_fakelag)
		ImGui::SliderInt("fakelag amount", &g_vars.misc_legit_aa_fakelag_amount, 1, 66);

	//ImGui::Checkbox("predict enemy lby updates", &g_vars.misc_predict_enemy_lby);
	ImGui::Checkbox("resolver", &g_vars.misc_resolver);


	/*if (g_vars.misc_resolver_type == 1) //build your own resolver
	{
		static int current_resolve_type = 0;
		static float add_to_angle = 180;
		//0 lby, 1 mlby, 2 eyeangs
		ImGui::Combo("base angle", &current_resolve_type, menu_lists::resolve_base_angles);
		ImGui::SliderFloat("add angle", &add_to_angle, -180, 180);

		if (ImGui::Button("push back choice"))
			resolver->resolver_shot_style.emplace_back(std::pair(current_resolve_type, add_to_angle));

		if (ImGui::Button("clear current resolver"))
			resolver->resolver_shot_style.clear();

		if (resolver->resolver_shot_style.empty())
			return;

		for (int i = 0; i < resolver->resolver_shot_style.size(); i++)
		{
			auto z = resolver->resolver_shot_style.at(i);
			std::string current_resolver_state = std::to_string(i) + " " + menu_lists::resolve_base_angles.at(z.first) + " " + std::to_string(z.second);
			ImGui::Text(current_resolver_state.c_str());
		}
	}*/
}

void movement_tab()
{
	ImGui::Checkbox("auto jump", &g_vars.misc_bhop);
	if (g_vars.misc_bhop)
	{
		ImGui::SliderInt("jumps per tick", &g_vars.misc_jumps_per_tick, 1, 60);
		ImGui::SliderInt("bhop chance", &g_vars.misc_bhop_chance, 1, 100);
	}
	ImGui::Checkbox("auto strafe", &g_vars.misc_autostrafe);
	if (g_vars.misc_autostrafe)
	{
		ImGui::Combo("auto strafe type", &g_vars.misc_autostrafe_type, menu_lists::autostrafe_types);
	}
	if (g_vars.misc_autostrafe_type == 2)
	{
		ImGui::SliderInt("strafe optimizer %", &g_vars.misc_autostrafe_optimizer_chance, 0, 100);
	}
	ImGui::Checkbox("edge jump", &g_vars.misc_edge_jump);
	if (g_vars.misc_edge_jump)
	{
		ImGui::Hotkey("edge jump key", &g_vars.misc_edge_jump_key);
		ImGui::Checkbox("crouch jump", &g_vars.misc_edge_jump_crouch);
	}

	ImGui::Checkbox("walkbot", &g_vars.misc_walkbot);
	if (g_vars.misc_walkbot)
	{
		ImGui::Checkbox("walkbot walk", &g_vars.misc_walkbot_walk);
		if (g_vars.misc_walkbot_walk)
		{
			ImGui::Checkbox("walkbot adaptive", &g_vars.misc_walkbot_adaptive);
		}
		ImGui::Hotkey("walkbot add key", &g_vars.misc_walkbot_add_key);
		ImGui::Hotkey("walkbot erase key", &g_vars.misc_walkbot_erase_key);
	}

	ImGui::Spacing();
	ImGui::Text("tas run bot");
	if (r.recording_active()) {
		if (ImGui::Button("stop recording run"))
			r.stop_recording();
	}
	else if (r.rerecording_active()) {
		if (ImGui::Button("save re-recording"))
			r.stop_rerecording(true);

		if (ImGui::Button("clear rerecording"))
			r.stop_rerecording(false);
	}
	else {
		if (ImGui::Button("start recording run"))
			r.start_recording();

		if (!r.get_active_recording().empty()) {
			if (ImGui::Button("clear run frames"))
				r.get_active_recording().clear();

			if (p.playback_active() && ImGui::Button("stop playback"))
				p.stop_playback();

			if (!p.playback_active() && ImGui::Button("start playback"))
				p.start_playback(r.get_active_recording());
		}
	}

	ImGui::Checkbox("copy bot", &g_vars.misc_copy_bot);

	//ImGui::Checkbox("restrict speed", &g_vars.misc_silent_walk);
	//ImGui::Checkbox("circle strafe", &g_vars.misc_circle_strafe);

	//if(g_vars.misc_circle_strafe)
		//ImGui::Hotkey("circle strafe key", &g_vars.misc_circle_strafe_on_key);
}

void etc_misc_tab()
{
	ImGui::Checkbox("clantag", &g_vars.misc_clantag_changer);
	ImGui::Checkbox("thirdperson", &g_vars.misc_thirdperson);
	if (g_vars.misc_thirdperson)
	{
		ImGui::Hotkey("third person key", &g_vars.misc_thirdperson_key);
		ImGui::SliderInt("thirdperson distance", &g_vars.misc_thirdperson_dist, 1, 500);
	}
	ImGui::Checkbox("chat spam", &g_vars.misc_chatspam);
	ImGui::Checkbox("sonar", &g_vars.misc_sonar);

	ImGui::Checkbox("radar", &g_vars.misc_radar);
	ImGui::Checkbox("quickswitch after shot", &g_vars.misc_quickswitch);

	//ImGui::Checkbox("ragdoll velocity mod", &g_vars.misc_ragdoll_launcher);
	if (g_vars.misc_ragdoll_launcher)
	{
		//ImGui::SliderFloat("ragdoll velocity multipler", &g_vars.misc_ragdoll_launcher_force, 0, 10);
	}
	ImGui::Checkbox("infinite duck", &g_vars.misc_fast_duck);
	ImGui::Checkbox("remove scope", &g_vars.misc_remove_scope);

	//ImGui::Checkbox("custom models", &g_vars.misc_custom_models);
	ImGui::Checkbox("hide names", &g_vars.misc_hide_names);
	ImGui::Checkbox("anti votekick", &g_vars.misc_anti_votekick);
	//ImGui::Checkbox("hide ambient noises", &g_vars.misc_hide_ambient_sounds);
	ImGui::Checkbox("menu noises", &g_vars.misc_menu_sounds);
	ImGui::Checkbox("menu snow", &g_vars.misc_menu_snow);
	ImGui::Checkbox("auto derank", &g_vars.misc_auto_derank);
	//ImGui::Checkbox("rank reveal", &g_vars.misc_rank_reveal);

	static char steam_id_64[128] = "";
	ImGui::InputText("steam id", steam_id_64, IM_ARRAYSIZE(steam_id_64));
	if (ImGui::Button("add friend", ImVec2(60, 30)))
	{
		const auto v = std::string(steam_id_64);
		try
		{
			const auto friend_id = std::stoll(v);
			modified_friends_list::friends.emplace_back(friend_id);
			push_notifications::Push("added STEAMID-" + v + " to modified friends list");
		}
		catch(std::exception& e)
		{
			push_notifications::Push("not a valid id.");
		}
	}

	if(ImGui::Button("reload friends list from file"))
	{
		modified_friends_list::load_friends_from_file();
		push_notifications::Push("reloaded friends_list.json");
	}

	if(ImGui::Button("save friends list to file"))
	{
		modified_friends_list::save_friends_to_file();
		push_notifications::Push("saved friends list to friends_list.json");
	}

	//if (ImGui::Button("fake timeout name", ImVec2(100, 30)))
	//{
	//	Utils::SetName("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n Press F1 To Resume Game! \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	//	push_notifications::Push("name set to fake timeout name");
	//}

	const static auto invite_spam = [=]() -> void {
#pragma pack(push, 1)
		struct item_t {
			uint16_t idx_next_0, unk_idx_2, idx_prev_4, unk_val_6;
			uint64_t steam_id_8;
			uint32_t* unk_ptr_16;
		}; //Size: 0x0014
#pragma pack(pop)

		static const auto collection = *(uint32_t**)(Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), "8B 35 ? ? ? ? 66 3B D0 74 07").value() + 2);

		if (*collection) {
			auto invite_to_lobby = [](uint64_t id) {
				class IMatchFramework; // GetMatchSession:13
				class ISteamMatchmaking; // InviteUserToLobby:16

				using GetLobbyId = uint64_t(__thiscall*)(void*);
				using GetMatchSession = uintptr_t * (__thiscall*)(IMatchFramework*);
				using InviteUserToLobby = bool(__thiscall*)(ISteamMatchmaking*, uint64_t, uint64_t);

				static const auto match_framework = **reinterpret_cast<IMatchFramework***>(Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), "8B 0D ? ? ? ? 8B 01 FF 50 2C 8D 4B 18").value() + 0x2);
				static const auto steam_matchmaking = **reinterpret_cast<ISteamMatchmaking***>(Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), "8B 3D ? ? ? ? 83 EC 08 8B 4D 08 8B C4").value() + 0x2);

				const auto match_session = call_vfunc<GetMatchSession>(match_framework, 13)(match_framework);
				if (match_session) {
					const uint64_t my_lobby_id = call_vfunc<GetLobbyId>(match_session, 4)(match_session);
					call_vfunc<InviteUserToLobby>(steam_matchmaking, 16)(steam_matchmaking, my_lobby_id, id);
				}
			};

			auto max_index = ((uint16_t*)collection)[9];

			for (uint16_t i = 0; i <= max_index; ++i) {
				auto item = &((item_t*)*collection)[i];
				invite_to_lobby(item->steam_id_8);
			}
		}
	};
		
	if (ImGui::Button("invite spam"))
		invite_spam();

	ImGui::Spacing();
	if (ImGui::Button("detach", ImVec2(50, 30)))
		g_Unload = true;
}

void misc_tab()
{
	static int misc_tabs = 0;
	constexpr auto size_x_tab = (size_x / 3) - 13;

	if (ImGui::Button("antiaim", ImVec2(size_x_tab, 25)))
		misc_tabs = 0;
	
	ImGui::SameLine();
	if (ImGui::Button("movement", ImVec2(size_x_tab, 25)))
		misc_tabs = 1;

	ImGui::SameLine();
	if (ImGui::Button("etc ## misc", ImVec2(size_x_tab, 25)))
		misc_tabs = 2;

	ImGui::Separator();

	switch (misc_tabs)
	{
	case 0: aa_tab(); break;
	case 1: movement_tab(); break;
	case 2: etc_misc_tab(); break;
	}
}

void skinchanger_tab()
{
	ImGui::Checkbox("skinchanger", &g_vars.skinchanger_enabled);

	if (!g_vars.skinchanger_enabled)
		return;

	static int current_weapon;

	ImGui::Combo("weapon", &current_weapon, menu_lists::weapon_list_names);

	ImGui::SliderInt("skin id", &skinchanger.info[menu_lists::weapon_list.at(current_weapon).second].skin_id, 1, 1500);
	ImGui::SliderFloat("wear", &skinchanger.info[menu_lists::weapon_list.at(current_weapon).second].wear, 0.00001, 1);
	ImGui::SliderInt("seed", &skinchanger.info[menu_lists::weapon_list.at(current_weapon).second].seed, 1, 100);

	//doesnt seem to work
	if (g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
	{
		if (ImGui::Button("force full update"))
		{
			//g_ClientState->m_nDeltaTick = -1;
			//g_EngineClient->ExecuteClientCmd("cl_fullupdate");

			const static auto force_full_update = []() -> void
			{
				static auto clear_hud_weapon_icon_fn = reinterpret_cast<std::int32_t(__thiscall*)(void*, std::int32_t)>(Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), ("55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 2C")).value());

				auto element = Utils::find_hud_element<std::uintptr_t*>(("CCSGO_HudWeaponSelection"));

				auto hud_weapons = reinterpret_cast<hud_weapons_t*>(std::uintptr_t(element) - 0xA0);
				if (hud_weapons == nullptr)
					return;

				if (!*hud_weapons->get_weapon_count())
					return;

				for (std::int32_t i = 0; i < *hud_weapons->get_weapon_count(); i++)
					i = clear_hud_weapon_icon_fn(hud_weapons, i);

				g_ClientState->force_full_update();
			};
			force_full_update();

		}
	}
}

void Menu::Render()
{
	ImGui::GetIO().MouseDrawCursor = _visible;

	static bool did_once = false;
	if (!_visible)
	{
		did_once = false;
		return;
	}

	if (!did_once)
	{
		if (g_vars.misc_menu_sounds)
			PlaySound((LPCWSTR)sound_effect_wav, NULL, SND_MEMORY | SND_ASYNC);

		did_once = true;
	}

	const auto str = "									VER$ACE - " + info_lp::username;
	ImGui::SetNextWindowSizeConstraints(ImVec2(size_x, size_y), ImVec2(size_x, size_y));
	if (ImGui::Begin(str.c_str(), &_visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		static int tabs = 0;
		constexpr auto tab_size_x = (size_x / 7) - 15;

		if (ImGui::Button("aim", ImVec2(tab_size_x, 30)))
			tabs = 0;

		ImGui::SameLine();
		if (ImGui::Button("trigger", ImVec2(tab_size_x, 30)))
			tabs = 1;

		ImGui::SameLine();
		if (ImGui::Button("visuals", ImVec2(tab_size_x, 30)))
			tabs = 2;

		ImGui::SameLine();
		if (ImGui::Button("players", ImVec2(tab_size_x, 30)))
			tabs = 3;

		ImGui::SameLine();
		if (ImGui::Button("skins", ImVec2(tab_size_x, 30)))
			tabs = 4;

		ImGui::SameLine();
		if (ImGui::Button("misc", ImVec2(tab_size_x, 30)))
			tabs = 5;

		ImGui::SameLine();
		if (ImGui::Button("configs", ImVec2(tab_size_x, 30)))
			tabs = 6;

		ImGui::Separator();

		switch (tabs)
		{
		case 0: aimbot_tab(); break;
		case 1: triggerbot_tab(); break;
		case 2: visual_tab(); break;
		case 3: players_tab(); break;
		case 4: skinchanger_tab(); break;
		case 5: misc_tab(); break;
		case 6: config_tab(); break;
		}
		ImGui::End();
	}
}

void Menu::Show()
{
	_visible = true;
	cl_mouseenable->SetValue(false);
}

void Menu::Hide()
{
	_visible = false;
	cl_mouseenable->SetValue(true);
}

void Menu::Toggle()
{
	cl_mouseenable->SetValue(_visible);
	_visible = !_visible;
}

void Menu::create_style()
{
	ImGuiStyle * style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

	ImGuiIO& io = ImGui::GetIO();
	io.FontDefault = io.Fonts->AddFontFromMemoryTTF(blocky, blocky_size, 9);
}

