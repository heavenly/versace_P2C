#pragma once
#include "valve_sdk/csgostructs.hpp"

namespace visuals_pt
{
	void draw_string(Color col, const char* str, int x, int y);

	namespace Player
	{
		bool Begin(C_BasePlayer* pl);

		Vector get_best_origin();
		bool is_completely_dormant();
		
		void render_box();
		void render_name();
		void render_health();
		void render_armor();
		void render_weapon();
		void render_ammo_bar();
		void render_snapline();
		void render_vulnerabilities();
		void draw_hitboxes(C_BasePlayer* entity, Color c);
		void draw_origin_box();
		void draw_money_esp();
		void draw_viewangles();
		void draw_distance();
		void draw_bone_esp();
		void render_defuser();
		void render_player_bomb_interactions();

		void draw_vs_user();
	}

	namespace Misc
	{
		void render_crosshair();
		void render_offscreen_esp();
		void render_planted_c4(C_BaseEntity* ent);
		void spectator_list();
		void third_person();
		void draw_logo_bar();
		void draw_aimbot_target();
		void draw_spread_crosshair();
		void draw_taser_range();
		void draw_autowall_damage();
		void draw_hud_features();
		void draw_fov_ring();
		void draw_intro();
		void draw_dangerzone_items(C_BaseEntity* ent);
	}

	bool create_fonts();
	void destroy_fonts();
	void draw_3d_box(float scalar, QAngle angles, Vector middle_origin, Color outline);
}