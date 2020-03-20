#include "js_functions.hpp"
#include "javascript_engine/duktape/duk_config.h"
#include "notification.h"
#include "local_info.h"
#include <filesystem>
#include <fstream>

c_js_funcs js_funcs;
#define non_stat duk_ret_t

non_stat c_js_funcs::push_notif(duk_ctx)
{
	push_notifications::Push(duk_to_string(ctx, 0));
	return 0;
}

non_stat c_js_funcs::get_current_user(duk_ctx)
{
	duk_push_string(ctx, info_lp::username.c_str());
	return 1;
}

non_stat c_js_funcs::print_to_console(duk_ctx)
{
	g_CVar->ConsolePrintf(duk_to_string(ctx, 0));
	return 0;
}

non_stat c_js_funcs::execute_client_cmd(duk_ctx)
{
	g_EngineClient->ExecuteClientCmd(duk_to_string(ctx, 0));
	return 0;
}

non_stat c_js_funcs::print_to_cmd_line(duk_ctx)
{
	auto to_print = duk_to_string(ctx, 0);
	Utils::ConsolePrint(to_print);
	return 0;
}


//entity stuff
non_stat c_js_funcs::get_max_clients(duk_ctx)
{
	duk_push_int(ctx, g_EngineClient->GetMaxClients());
	return 1;
}

non_stat c_js_funcs::entity_hp(duk_ctx)
{
	auto index = duk_to_int(ctx, 0);
	auto p = C_BasePlayer::GetPlayerByIndex(index);
	
	duk_push_int(ctx, p->m_iHealth());
	return 1;
}

non_stat c_js_funcs::entity_dormant(duk_ctx)
{
	auto index = duk_to_int(ctx, 0);
	auto p = C_BasePlayer::GetPlayerByIndex(index);
	duk_push_boolean(ctx, p->is_dormant());
	return 1;
}

non_stat c_js_funcs::get_eye_pos(duk_ctx)
{
	//this function is retarded and doesn't work well. fix
	auto index = duk_to_int(ctx, 0);
	auto p = C_BasePlayer::GetPlayerByIndex(index);
	auto v = p->get_eye_pos();
	auto arr_idx = duk_push_array(ctx);
	duk_push_int(ctx, v.x);
	duk_put_prop_index(ctx, arr_idx, 0);
	duk_push_int(ctx, v.y);
	duk_put_prop_index(ctx, arr_idx, 1);
	duk_push_int(ctx, v.z);
	duk_put_prop_index(ctx, arr_idx, 2);
	return 1;
}

non_stat c_js_funcs::entity_is_bot(duk_ctx)
{
	auto index = duk_to_int(ctx, 0);
	auto p = C_BasePlayer::GetPlayerByIndex(index);
	auto player_info = p->get_player_info();
	duk_push_boolean(ctx, player_info.fakeplayer);
	return 1;
}

non_stat c_js_funcs::entity_get_name(duk_ctx)
{
	auto index = duk_to_int(ctx, 0);
	auto p = C_BasePlayer::GetPlayerByIndex(index);
	auto player_info = p->get_player_info();
	duk_push_string(ctx, player_info.szName);
	return 1;
}

non_stat c_js_funcs::entity_get_local(duk_ctx)
{
	auto local_index = g_LocalPlayer->EntIndex();
	duk_push_int(ctx, local_index);
	return 1;
}

non_stat c_js_funcs::entity_get_team(duk_ctx)
{
	auto index = duk_to_int(ctx, 0);
	auto p = C_BasePlayer::GetPlayerByIndex(index);
	duk_push_int(ctx, p->m_iTeamNum());
	return 1;
}

non_stat c_js_funcs::entity_is_null(duk_ctx)
{
	auto index = duk_to_int(ctx, 0);
	auto p = C_BasePlayer::GetPlayerByIndex(index);
	duk_push_boolean(ctx, p == nullptr);
	return 1;
}

non_stat c_js_funcs::entity_get_flags(duk_ctx)
{
	auto index = duk_to_int(ctx, 0);
	auto p = C_BasePlayer::GetPlayerByIndex(index);
	duk_push_int(ctx, p->m_fFlags());
	return 1;
}

non_stat c_js_funcs::set_view_angles(duk_ctx)
{
	auto x = duk_to_number(ctx, 0);
	auto y = duk_to_number(ctx, 1);
	auto z = duk_to_number(ctx, 2);
	global_vars::cmd->viewangles = QAngle(x, y, z);
	return 0;
}

non_stat c_js_funcs::player_jump(duk_ctx)
{
	global_vars::cmd->buttons |= IN_JUMP;
	return 0;
}

non_stat c_js_funcs::player_crouch(duk_ctx)
{
	global_vars::cmd->buttons |= IN_DUCK;
	return 0;
}

non_stat c_js_funcs::get_tick_count(duk_ctx)
{
	duk_push_int(ctx, global_vars::cmd->tick_count);
	return 1;
}

non_stat c_js_funcs::set_tick_count(duk_ctx)
{
	auto new_tick_count = duk_to_int(ctx, 0);
	global_vars::cmd->tick_count = new_tick_count;
	return 0;
}

non_stat c_js_funcs::gv_curtime(duk_ctx)
{
	duk_push_number(ctx, g_GlobalVars->curtime);
	return 1;
}

non_stat c_js_funcs::engine_is_in_game(duk_ctx)
{
	duk_push_boolean(ctx, g_EngineClient->IsInGame());
	return 1;
}

non_stat c_js_funcs::engine_set_clantag(duk_ctx)
{
	auto new_clantag = duk_to_string(ctx, 0);
	Utils::SetClantag(new_clantag);
	return 0;
}

non_stat c_js_funcs::get_fsn_stage(duk_ctx)
{
	duk_push_int(ctx, global_vars::stage);
	return 1;
}

non_stat c_js_funcs::draw_line(duk_ctx)
{
	auto x0 = duk_to_number(ctx, 0);
	auto y0 = duk_to_number(ctx, 1);
	auto x1 = duk_to_number(ctx, 2);
	auto y1 = duk_to_number(ctx, 3);
	g_VGuiSurface->DrawLine(x0, y0, x1, y1);
	return 0;
}

non_stat c_js_funcs::draw_set_color(duk_ctx)
{
	auto r = duk_to_number(ctx, 0);
	auto g = duk_to_number(ctx, 1);
	auto b = duk_to_number(ctx, 2);
	auto a = duk_to_number(ctx, 3);
	g_VGuiSurface->DrawSetColor(r, g, b, a);
	return 0;
}

non_stat c_js_funcs::draw_set_text_color(duk_ctx)
{
	auto r = duk_to_number(ctx, 0);
	auto g = duk_to_number(ctx, 1);
	auto b = duk_to_number(ctx, 2);
	auto a = duk_to_number(ctx, 3);
	g_VGuiSurface->DrawSetTextColor(r, g, b, a);
	return 0;
}

non_stat c_js_funcs::draw_text(duk_ctx)
{
	auto x = duk_to_int(ctx, 0);
	auto y = duk_to_int(ctx, 1);
	auto to_draw = duk_to_string(ctx, 2);
	wchar_t formatted[512] = { '\0' };
	wsprintfW(formatted, L"%S", to_draw);
	g_VGuiSurface->DrawSetTextPos(x, y);
	g_VGuiSurface->DrawPrintText(formatted, wcslen(formatted));
	return 0;
}

non_stat c_js_funcs::draw_filled_rect(duk_ctx)
{
	auto x0 = duk_to_int(ctx, 0);
	auto y0 = duk_to_int(ctx, 1);
	auto x1 = duk_to_int(ctx, 2);
	auto y1 = duk_to_int(ctx, 3);
	g_VGuiSurface->DrawFilledRect(x0, y0, x1, y1);
	return 0;
}

non_stat c_js_funcs::draw_circle(duk_ctx)
{
	auto x = duk_to_int(ctx, 0);
	auto y = duk_to_int(ctx, 1);
	auto radius = duk_to_int(ctx, 2);
	auto segments = duk_to_int(ctx, 3);
	g_VGuiSurface->DrawOutlinedCircle(x, y, radius, segments);
	return 0;
}

non_stat c_js_funcs::get_cursor_pos(duk_ctx)
{
	POINT pt;
	GetCursorPos(&pt);
	auto arr_idx = duk_push_array(ctx);
	duk_push_int(ctx, pt.x);
	duk_put_prop_index(ctx, arr_idx, 0);
	duk_push_int(ctx, pt.y);
	duk_put_prop_index(ctx, arr_idx, 1);
	return 1;
}

non_stat c_js_funcs::get_key_state(duk_ctx)
{
	auto key_num = duk_to_int(ctx, 0);
	duk_push_boolean(ctx, GetKeyState(key_num));
	return 1;
}

non_stat c_js_funcs::get_async_key_state(duk_ctx)
{
	auto key_num = duk_to_int(ctx, 0);
	duk_push_boolean(ctx, GetAsyncKeyState(key_num));
	return 1;
}

non_stat c_js_funcs::play_sound(duk_ctx)
{
	auto path = duk_to_string(ctx, 0);
	if (!std::filesystem::exists(path))
		return 0;
	PlaySoundA(path, NULL, SND_FILENAME | SND_ASYNC);
	return 0;
}

void c_js_funcs::setup_js()
{
	const auto add_function = [&](const duk_c_function fn, const int args, const std::string& fn_name) -> void
	{
		duk_push_c_function(global_vars::ctx, fn, args);
		duk_put_global_string(global_vars::ctx, fn_name.c_str());
	};

	//general stuff
	add_function(push_notif, 1, "show_notification"); //ctx, function name, amount of input args
	add_function(get_current_user, 0, "current_username");
	add_function(print_to_console, 1, "print_to_console");
	add_function(execute_client_cmd, 1, "execute_client_cmd");
	add_function(print_to_cmd_line, 1, "print_to_cmd_line");

	//entity functions - this should be converted to a class
	add_function(get_max_clients, 0, "get_max_clients");
	add_function(entity_hp, 1, "get_ent_hp");
	add_function(entity_dormant, 1, "get_ent_dormant");
	add_function(get_eye_pos, 1, "get_ent_eye_pos");
	add_function(entity_is_bot, 1, "get_ent_bot");
	add_function(entity_get_name, 1, "get_ent_name");
	add_function(entity_get_local, 0, "get_ent_local_index");
	add_function(entity_get_team, 1, "get_ent_team");
	add_function(entity_is_null, 1, "get_ent_nullptr");
	add_function(entity_get_flags, 1, "get_ent_flags");

	//createmove functions - this should also be a class, right now these are just methods of CUserCmd
	add_function(set_view_angles, 3, "set_view_angles");
	add_function(player_jump, 0, "local_jump");
	add_function(player_crouch, 0, "local_crouch");
	add_function(get_tick_count, 0, "cm_tickcount");
	add_function(set_tick_count, 1, "set_cm_tickcount");

	//engine and globalvars functions
	add_function(gv_curtime, 0, "get_curtime");
	add_function(engine_is_in_game, 0, "get_in_game");
	add_function(engine_set_clantag, 1, "set_clantag");
	add_function(get_fsn_stage, 0, "get_fsn_stage");

	//paint functions
	add_function(draw_line, 4, "draw_line");
	add_function(draw_set_color, 4, "draw_set_color");
	add_function(draw_set_text_color, 4, "draw_set_text_color");
	add_function(draw_text, 3, "draw_text");
	add_function(draw_filled_rect, 4, "draw_filled_rect");
	add_function(draw_circle, 4, "draw_circle");

	//keyboard and mouse functions
	add_function(get_cursor_pos, 0, "get_cursor_pos");
	add_function(get_key_state, 1, "get_key_state");
	add_function(get_async_key_state, 1, "get_async_key_state");
	add_function(play_sound, 1, "play_sound");
}

std::string c_js_funcs::read_script_from_file(const std::string path)
{
	std::fstream script_file;
	script_file.open(path, std::ios::in); //open a file to perform read operation using file object
	std::string script;
	if (script_file.is_open()) {   //checking whether the file is open
		std::string tp;
		while (std::getline(script_file, tp)) {  //read data from file object and put it into string.
			script += tp;  //print the data of the string
		}
		script_file.close();   //close the file object.
	}
	return script;
}

std::vector<std::string> c_js_funcs::get_js_files()
{
	const auto scripts_path = "C:\\VER$ACE\\scripts\\";
	std::vector<std::string> scripts = {};
	for (auto& p : std::filesystem::recursive_directory_iterator(scripts_path))
	{
		if (p.path().extension() == ".js")
		{
			std::string filename = p.path().filename().string();
			scripts.push_back(filename);
		}
	}

	return scripts;
}
