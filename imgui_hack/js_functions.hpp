#pragma once
#include <string>
#include "valve_sdk/csgostructs.hpp"
#include "javascript_engine/duktape/duk_config.h"

#define ret_type static duk_ret_t
#define duk_ctx duk_context* ctx

class c_js_funcs
{
private:
	ret_type push_notif(duk_ctx);
	ret_type get_current_user(duk_ctx);
	ret_type print_to_console(duk_ctx);
	ret_type execute_client_cmd(duk_ctx);
	ret_type print_to_cmd_line(duk_ctx);
	
	ret_type get_max_clients(duk_ctx);
	ret_type entity_hp(duk_ctx);
	ret_type entity_dormant(duk_ctx);
	ret_type get_eye_pos(duk_ctx);
	ret_type entity_is_bot(duk_ctx);
	ret_type entity_get_name(duk_ctx);
	ret_type entity_get_local(duk_ctx);
	ret_type entity_get_team(duk_ctx);
	ret_type entity_is_null(duk_ctx);
	ret_type entity_get_flags(duk_ctx);
	
	ret_type set_view_angles(duk_ctx);
	ret_type player_jump(duk_ctx);
	ret_type player_crouch(duk_ctx);
	
	ret_type get_tick_count(duk_ctx);
	ret_type set_tick_count(duk_ctx);
	ret_type gv_curtime(duk_ctx);
	ret_type engine_is_in_game(duk_ctx);
	ret_type engine_set_clantag(duk_ctx);
	ret_type get_fsn_stage(duk_ctx);
	
	ret_type draw_line(duk_ctx);
	ret_type draw_set_color(duk_ctx);
	ret_type draw_set_text_color(duk_ctx);
	ret_type draw_text(duk_ctx);
	ret_type draw_filled_rect(duk_ctx);
	ret_type draw_circle(duk_ctx);

	ret_type get_cursor_pos(duk_ctx);
	ret_type get_key_state(duk_ctx);
	ret_type get_async_key_state(duk_ctx);
	ret_type play_sound(duk_ctx);
public:
	//call in dllmain
	static void setup_js();

	static std::string read_script_from_file(const std::string path);
	static std::vector<std::string> get_js_files();
private:
};

extern c_js_funcs js_funcs;