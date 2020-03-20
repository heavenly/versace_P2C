#pragma once
#include "hooks.hpp"
#include "include/minhook/minhook.h"
#include <intrin.h>  
#include <chrono>
#include <algorithm>
#include "render.h"
#include "menu.hpp"
#include "options.hpp"
#include "helpers/input.hpp"
#include "helpers/utils.hpp"
#include "helpers/math.hpp"
#include "features/chams.hpp"
#include "autostrafe.h"
#include "aimbot.h"
#include "player_matrix.h"
#include "fixmove.h"
#include "legit_aa.h"
#include "rcs.h"
#include "anticheat.h"
#include "backtrack.h"
#include "visuals_pt.h"
#include "chat_spam.h"
#include "sonar.h"
#include "radar.h"
#include "beams.h"
#include "animfix.h"
#include "glow.h"
#include "grenades.h"
#include "trigger.h"
#include "skinchanger.h"
#include "far_esp.h"
#include "misc_features.h"
#include "grenade_prediction.h"
#include "walkbot.h"
#include "tas_bot.h"
#include "resolver.h"
#include "nightmode.h"
#include "circle_strafe.h"
#include "radar_far_esp.h"


//gui stuff
#include "particles.h"
#include "notification.h"

//listeners
#include "player_hurt.h"
#include "bullet_impact.h"
#include "vote_listener.h"
#include "weapon_fire.h"
#include "item_purchase.h"

//info storage
#include "global.h"
#include "local_info.h"
#include "friends_list.h"
#include "json.h"
#include "http_client.h"
#include "imgui/directx9/imgui_impl_dx9.h"

//js
#include "javascript_engine/duktape/duktape.h"
#include "js_functions.hpp"
#include "js_loader.h"

#pragma intrinsic(_ReturnAddress) 
static auto fix_move = c_fix_move();
static auto aimbot = c_aimbot();
static auto autostrafe = autostrafer();
static auto rcs = c_recoil();
static auto chat_spam = c_chat_spam();
static auto sonar = c_sonar();
static auto radar = c_radar();
static auto beams = c_beams();
static auto animfix = c_animfix();
static auto misc_feats = c_misc_features();
static auto grenade_prediction = grenade_pred();
static auto walkbot = c_walkbot();
static auto legit_aa = c_antiaim();
static auto nightmode = c_nightmode();
static auto circle_strafe = c_circle_strafe();
long long start_epoch = 0;

namespace Hooks
{
	vfunc_hook hlclient_hook;
	vfunc_hook direct3d_hook;
	vfunc_hook vguipanel_hook;
	vfunc_hook vguisurf_hook;
	vfunc_hook sound_hook;
	vfunc_hook mdlrender_hook;
	vfunc_hook clientmode_hook;
	vfunc_hook viewrender_hook;
	vfunc_hook renderview_hook;
	vfunc_hook findmdl_hook;
	vfunc_hook engine_hook;
	vfunc_hook steam_friends_hook;
	vfunc_hook present_hook;
	
	void Initialize()
	{
		start_epoch = Utils::get_epoch_time();
		constexpr auto client =			"client_panorama.dll";
		constexpr auto engine =			"engine.dll";
		constexpr auto dx9 =			"shaderapidx9.dll";
		constexpr auto vgui2 =			"vgui2.dll";
		constexpr auto vgui_mat_surf =	"vguimatsurface.dll";
		constexpr auto data_cache =		"datacache.dll";
		constexpr auto steam_api =		"steam_api.dll";

		hlclient_hook.setup(g_CHLClient, client);
		direct3d_hook.setup(g_D3DDevice9, dx9);
		vguipanel_hook.setup(g_VGuiPanel, vgui2);
		vguisurf_hook.setup(g_VGuiSurface, vgui_mat_surf);
		sound_hook.setup(g_EngineSound, engine);
		mdlrender_hook.setup(g_MdlRender);
		clientmode_hook.setup(g_ClientMode, client);
		viewrender_hook.setup(g_ViewRender);
		renderview_hook.setup(g_RenderView);
		findmdl_hook.setup(g_MdlCache);
		engine_hook.setup(g_EngineClient, engine);
		steam_friends_hook.setup(g_SteamFriends);

		direct3d_hook.hook_index(index::EndScene, hkEndScene);
		direct3d_hook.hook_index(index::Reset, hkReset);

		hlclient_hook.hook_index(index::FrameStageNotify, hkFrameStageNotify);
		hlclient_hook.hook_index(index::CreateMove, hkCreateMove_Proxy);

		vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);

		vguisurf_hook.hook_index(index::LockCursor, hkLockCursor);

		mdlrender_hook.hook_index(index::DrawModelExecute, hkDrawModelExecute);

		clientmode_hook.hook_index(index::DoPostScreenSpaceEffects, hkDoPostScreenEffects);
		clientmode_hook.hook_index(index::OverrideView, hkOverrideView);

		renderview_hook.hook_index(index::SceneEnd, hkSceneEnd);

		hlclient_hook.hook_index(index::DispatchUserMsg, DispatchUserMessage_h);

		findmdl_hook.hook_index(index::FindMDL, hk_findmdl);

		engine_hook.hook_index(index::GetPlayerInfo, hkGetPlayerInfo);

		steam_friends_hook.hook_index(index::GetFriendCount, hk_GetFriendCount);
		steam_friends_hook.hook_index(index::GetFriendByIndex, hk_GetFriendByIndex);
		steam_friends_hook.hook_index(index::GetFriendPersonaState, hk_GetFriendPersonaState);

		//broke
		//clientmode_hook.hook_index(index::ShouldDrawViewModel, hkDrawViewModel);

		//Utils::AttachConsole();
		visuals_pt::create_fonts();

		//listeners
		player_hurt_event::Get().register_self();
		bullet_impact_event::Get().register_self();
		grenade_detonate_event::Get().register_self();
		call_vote_event::Get().register_self();
		weapon_fire_event::Get().register_self();
		item_purchase_event::Get().register_self();

		c_js_funcs::setup_js();
	}
	
	void Shutdown()
	{
		duk_destroy_heap(global_vars::ctx);
		dot_destroy();
		player_hurt_event::Get().unregister_self();
		bullet_impact_event::Get().unregister_self();
		grenade_detonate_event::Get().unregister_self();
		call_vote_event::Get().unregister_self();
		weapon_fire_event::Get().unregister_self();
		item_purchase_event::Get().unregister_self();
		far_esp::Get().clear();
		Glow::clear_glow();

		hlclient_hook.unhook_all();
		direct3d_hook.unhook_all();
		vguipanel_hook.unhook_all();
		vguisurf_hook.unhook_all();
		mdlrender_hook.unhook_all();
		clientmode_hook.unhook_all();
		engine_hook.unhook_all();
		steam_friends_hook.unhook_all();
	}

	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		auto oReset = direct3d_hook.get_original<Reset>(index::Reset);

		Menu::Get().OnDeviceLost();

		auto hr = oReset(device, pPresentationParameters);

		if (hr >= 0) {
			Menu::Get().OnDeviceReset();
			visuals_pt::create_fonts();
		}

		return hr;
	}

	void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
	{
		auto oCreateMove = hlclient_hook.get_original<CreateMove>(index::CreateMove);

		oCreateMove(g_CHLClient, sequence_number, input_sample_frametime, active);

		auto cmd = g_Input->GetUserCmd(sequence_number);
		auto verified = g_Input->GetVerifiedCmd(sequence_number);

		if (!cmd || !cmd->command_number)
			return;

		const auto set_triggers = [&]() -> void
		{
			info_lp::weapon_fire_triggered = false;
			info_lp::player_hurt_triggered = false;
			info_lp::bullet_impact_triggered = false;
		};

		if (g_LocalPlayer && g_EngineClient->IsInGame() && g_EngineClient->IsConnected() && cmd)
		{
			for (size_t i = 1; i < g_EngineClient->GetMaxClients(); i++)
			{

				C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
				if (!entity)
					continue;

				if (!entity->is_alive() || entity->is_dormant())
					continue;

				//cache bonematrix for cheat optimization
				entity->SetupBones(player_matrixes[i].matrix, 128, BONE_USED_BY_ANYTHING, entity->GetSimulationTime());

				if (g_vars.esp_on_key > 0 && !GetAsyncKeyState(g_vars.esp_on_key))
					continue;

				beams.draw_rings(entity);
			}

			anticheat.do_createmove_log();
			chat_spam.do_spam();
			//this is here so that this function will work properly
			walkbot.get_best_position();
			//added something in here to fix the real lag the fakelag causes
			misc_feats.change_clantag();

			if (g_LocalPlayer->is_alive())
			{
				set_triggers();

				g_Prediction->SetLocalViewangles(cmd->viewangles);
				backtrack.register_tick(cmd);
				backtrack.clean_records(cmd);
				autostrafe.do_strafes(cmd);
				//circle_strafe->circle_strafer_main(cmd);
				walkbot.walk(cmd);

				if (cmd->buttons != 0 && p.playback_active())
				{
					r.start_rerecording(p.get_current_frame());
					p.stop_playback();
				}

				r.start_recording(cmd);
				p.start_playback(cmd);

				fix_move.Start(cmd);
				autostrafe.auto_jump(cmd);
				autostrafe.edge_jump(cmd);
				aimbot.do_aimbot(cmd);
				backtrack.Begin(cmd);
				backtrack.End();
				trigger.do_trigger(cmd);
				rcs.do_recoil(cmd);

				//remember to add this to the menu sometime
				//legit_aa.anti_trigger(cmd);
				//legit_aa.anti_trigger_choke(bSendPacket);
				legit_aa.do_fakelag(cmd, bSendPacket);
				legit_aa.do_antiaim(cmd, bSendPacket);

				//if (g_vars.misc_rank_reveal && cmd->buttons & IN_SCORE)
					//Utils::RankRevealAll();

				misc_feats.do_quickswitch(cmd);
				misc_feats.infinite_crouch(cmd);
				//broken
				//misc_feats.get()->restrict_speed(cmd);
				//grenade_prediction->Tick(cmd->buttons);
				fix_move.Stop(cmd);
				misc_feats.copy_movement_bot(cmd);
				misc_feats.log_quickswitch(cmd);
				misc_feats.set_fakelag_limit();
				misc_feats.crash_server();

				global_vars::cmd = cmd;
				c_js_loader::execute_scripts(create_move);

				Math::Clamp(cmd->viewangles);

				cmd->forwardmove = std::clamp(cmd->forwardmove, -450.f, 450.f);
				cmd->sidemove = std::clamp(cmd->sidemove, -450.f, 450.f);
				cmd->upmove = std::clamp(cmd->upmove, -450.f, 450.f);

				cmd->viewangles.yaw = std::clamp(cmd->viewangles.yaw, -180.f, 180.f);
				cmd->viewangles.pitch = std::clamp(cmd->viewangles.pitch, -89.f, 89.f);
			}
		}
		verified->m_cmd = *cmd;
		verified->m_crc = cmd->GetChecksum();
	}

	__declspec(naked) void __stdcall hkCreateMove_Proxy(int sequence_number, float input_sample_frametime, bool active)
	{
		__asm
		{
			push ebp
			mov  ebp, esp
			push ebx
			lea  ecx, [esp]
			push ecx
			push dword ptr[active]
			push dword ptr[input_sample_frametime]
			push dword ptr[sequence_number]
			call Hooks::hkCreateMove
			pop  ebx
			pop  ebp
			retn 0Ch
		}
	}

	void __stdcall hkPaintTraverse(vgui::VPANEL panel, bool forceRepaint, bool allowForce)
	{
		static auto panel_id = vgui::VPANEL{ 0 };
		static auto panel_zoom_id = vgui::VPANEL{ 0 };
		static auto oPaintTraverse = vguipanel_hook.get_original<PaintTraverse>(index::PaintTraverse);

		const auto panel_name = g_VGuiPanel->GetName(panel);

		if (!panel_zoom_id)
		{
			if (!strcmp(panel_name, "HudZoom"))
				panel_zoom_id = panel;
		}
		else if (panel == panel_zoom_id && g_vars.misc_remove_scope)
			return;

		oPaintTraverse(g_VGuiPanel, panel, forceRepaint, allowForce);

		if (!panel_id) {
			if (!strcmp(panel_name, "FocusOverlayPanel"))
				panel_id = panel;
		}
		else if (panel_id == panel) {
			int screen_width, screen_height;
			g_EngineClient->GetScreenSize(screen_width, screen_height);

			//see func and fix
			//visuals_pt::Misc::draw_intro();

			misc_feats.auto_derank(); //only reason this is here is because PT is always called :clown:

			if (Menu::Get().IsVisible())
				dot_draw();

			info_lp::spec_list_size = 0;

			//visuals_pt::Misc::draw_logo_bar();
			visuals_pt::Misc::draw_hud_features();
			push_notifications::Update();
			c_js_loader::execute_scripts(paint_traverse);

			if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected() && !g_EngineClient->IsTakingScreenshot()) {

				if (!g_LocalPlayer)
					return;

				nightmode.do_nightmode();

				if (!g_vars.esp_enabled)
					return;

				if (g_vars.esp_on_key > 0 && !GetAsyncKeyState(g_vars.esp_on_key))
					return;

				if (g_LocalPlayer->is_alive())
				{
					player_hurt_event::Get().paint();
					visuals_pt::Misc::draw_spread_crosshair();
					visuals_pt::Misc::draw_aimbot_target();
					visuals_pt::Misc::render_offscreen_esp();
					visuals_pt::Misc::draw_taser_range();
					//grenade_prediction->Paint();
					visuals_pt::Misc::spectator_list();
					walkbot.add_to_vector();
					visuals_pt::Misc::draw_fov_ring();
					visuals_pt::Misc::draw_autowall_damage();
					//draw scope lines
					if (g_LocalPlayer->m_bIsScoped() && g_vars.misc_remove_scope)
					{
						g_VGuiSurface->DrawSetColor(Color(0, 0, 0));
						g_VGuiSurface->DrawLine(0, screen_height / 2, screen_width, screen_height / 2);
						g_VGuiSurface->DrawLine(screen_width / 2, 0, screen_width / 2, screen_height);
					}
				}

				far_esp::Get().Start();
				for (auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i) {
					auto entity = C_BasePlayer::GetPlayerByIndex(i);

					if (!entity)
						continue;

					if (entity == g_LocalPlayer && !g_vars.esp_targets[2])
						continue;

					if (entity == g_LocalPlayer && !g_Input->m_fCameraInThirdPerson)
						continue;

					auto dist = [=](Vector a, Vector b) -> int
					{
						return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
					};

					if (dist(g_LocalPlayer->m_vecOrigin(), entity->m_vecOrigin()) > g_vars.esp_drawing_dist)
						continue;

					if (i < 65 && visuals_pt::Player::Begin(entity)) {
						if (g_vars.esp_player_snaplines) visuals_pt::Player::render_snapline();
						if (g_vars.esp_player_boxes)     visuals_pt::Player::render_box();
						if (g_vars.esp_player_weapons)   visuals_pt::Player::render_weapon();
						if (g_vars.esp_player_names)     visuals_pt::Player::render_name();
						if (g_vars.esp_player_health)    visuals_pt::Player::render_health();
						if (g_vars.esp_player_armour)    visuals_pt::Player::render_armor();
						if (g_vars.esp_player_vulnerabilities) visuals_pt::Player::render_vulnerabilities();
						if (g_vars.esp_origin_box)		visuals_pt::Player::draw_origin_box();
						if (g_vars.esp_player_money)	visuals_pt::Player::draw_money_esp();
						if (g_vars.esp_player_viewangles) visuals_pt::Player::draw_viewangles();
						if (g_vars.esp_distance) visuals_pt::Player::draw_distance();
						if (g_vars.esp_player_ammo_bar)	visuals_pt::Player::render_ammo_bar();
						if (g_vars.esp_player_c4 || g_vars.esp_player_defuser) visuals_pt::Player::render_defuser();
						//if (g_vars.esp_player_defusing_bar || g_vars.esp_player_planting_bar) visuals_pt::Player::render_player_bomb_interactions();
						if (g_vars.esp_player_skeleton) visuals_pt::Player::draw_bone_esp();
						if (g_vars.backtrack_skeleton) backtrack.draw_backtrack_skeleton(entity);
						if (g_vars.esp_draw_vs_user) visuals_pt::Player::draw_vs_user();
					}
					else if (entity->IsPlantedC4())
					{
						//if (g_vars.esp_planted_c4)
							//visuals_pt::Misc::render_planted_c4(entity);
					}
					else if (entity->IsLoot())
					{
						if (g_vars.esp_draw_dangerzone_items) visuals_pt::Misc::draw_dangerzone_items(entity);
					}
				}
			}
		}
	}

	int __stdcall hkDoPostScreenEffects(int a1)
	{
		auto oDoPostScreenEffects = clientmode_hook.get_original<DoPostScreenEffects>(index::DoPostScreenSpaceEffects);

		return oDoPostScreenEffects(g_ClientMode, a1);
	}

	void __stdcall hkFrameStageNotify(ClientFrameStage_t stage)
	{
		static auto oFSN = hlclient_hook.get_original<FrameStageNotify>(index::FrameStageNotify);

		if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected() && g_LocalPlayer)
		{
			//causes big fps damage
			//Glow::add_to_glow_mgr(stage);
			global_vars::stage = stage;
			c_js_loader::execute_scripts(frame_stage_notify);

			if (stage == FRAME_RENDER_START)
			{
				if (g_vars.misc_ragdoll_launcher)
				{
					for (size_t i = 1; i < g_EntityList->GetHighestEntityIndex(); i++) {
						auto entity = g_EntityList->GetClientEntity(i);

						if (!entity || entity->is_dormant())
							continue;

						if (entity->GetClientClass()->m_ClassID != ClassId::ClassId_CCSRagdoll && entity->GetClientClass()->m_ClassID != ClassId::ClassId_CRagdollProp)
							continue;

						auto ragdoll = (c_ragdoll*)entity;

						if (!ragdoll)
							continue;

						const auto force_multiplier = g_vars.misc_ragdoll_launcher_force;
						ragdoll->m_vecForce() *= force_multiplier;
						ragdoll->m_vecRagdollVelocity() *= force_multiplier;
					}
				}

				if (g_vars.esp_enabled && g_vars.vis_misc_grenade_circle)
					grenade_detonate_event::Get().paint();

				if (g_LocalPlayer->is_alive() && g_vars.esp_enabled && g_vars.vis_misc_bullet_beams)
					bullet_impact_event::Get().paint();

				//PV$ FIX
				for (size_t i = 1; i < g_EngineClient->GetMaxClients(); i++)
				{
					auto entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

					if (!entity)
						continue;

					if (!entity->is_valid())
						continue;

					*(int*)((uintptr_t)entity + 0xA30) = g_GlobalVars->framecount;
					*(int*)((uintptr_t)entity + 0xA28) = 0;
				}

				visuals_pt::Misc::third_person();
				if (g_vars.misc_no_flash)
					g_LocalPlayer->m_flFlashMaxAlpha() = 0;
				else if(g_LocalPlayer->m_flFlashMaxAlpha() == 0)
					g_LocalPlayer->m_flFlashMaxAlpha() = 255;

			}
			if (stage == FRAME_NET_UPDATE_START)
			{
				static bool set_wireframe = false;
				const static std::vector<const char*> smoke_mats =
				{
					"particle/vistasmokev1/vistasmokev1_fire",
					"particle/vistasmokev1/vistasmokev1_smokegrenade",
					"particle/vistasmokev1/vistasmokev1_emods",
					"particle/vistasmokev1/vistasmokev1_emods_impactdust",
				};

				const static auto set_wf = [=](bool status) -> void
				{
					for (auto mats : smoke_mats)
					{
						IMaterial* mat = g_MatSystem->FindMaterial(mats, TEXTURE_GROUP_OTHER);
						mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, status);
					}
				};
				
				if (g_vars.vis_misc_no_smoke)
				{					
					if (!set_wireframe)
					{
						set_wf(true);
						set_wireframe = true;
					}
				}
				else if(set_wireframe)
				{
					set_wf(false);
					set_wireframe = false;
				}

				if (g_LocalPlayer->is_alive())
				{
					sonar.do_sonar();
					radar.do_radar();
				}
			}

			if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
			{
				anticheat.fix_players();
				resolver.log_player_info();
				skinchanger.do_skin_change();
				anticheat.do_lby_pred();
				animfix.do_animfix();
			}

			oFSN(g_CHLClient, stage);
			if (stage == FRAME_NET_UPDATE_END)
			{
				c_radar_far_esp::cache_players(); //need to test / fix this
				
				for (size_t i = 1; i < g_EngineClient->GetMaxClients(); i++)
				{

					C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
					if (!entity)
						continue;

					if (!entity->is_alive() || entity->is_dormant())
						continue;

					if (g_vars.esp_on_key > 0 && !GetAsyncKeyState(g_vars.esp_on_key))
						continue;

					beams.draw_rings(entity);
				}
			}
		}
		else
			oFSN(g_CHLClient, stage);
	}

	void __stdcall hkLockCursor()
	{
		static auto orig_func = vguisurf_hook.get_original<LockCursor_t>(index::LockCursor);

		if (Menu::Get().IsVisible())
		{
			g_VGuiSurface->UnlockCursor();
			return;
		}

		orig_func(g_VGuiSurface);
	}

	void __fastcall hkSceneEnd(void* thisptr, void* edx)
	{
		static auto orig_scene_end = renderview_hook.get_original<SceneEnd>(index::SceneEnd);
		orig_scene_end(thisptr, edx);

		Glow::render_glow();

		for (int i = 1; i < g_EngineClient->GetMaxClients(); ++i) {
			auto ent = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

			if (!ent)
				continue;

			if (!ent->is_alive())
				continue;

			//i have no idea if this will work - far chams - didnt work
			g_MdlRender->ForcedMaterialOverride(nullptr);
			/*auto prev_origin = ent->get_abs_origin();
			auto record = far_esp::Get().sound_players[ent->EntIndex()];

			if (ent->is_dormant() && !record.dormant)
				ent->set_abs_origin(record.origin);

			ent->DrawModel(0x1, 255);

			if (ent->is_dormant() && !record.dormant)
				ent->set_abs_origin(prev_origin);
			*/

		}
	}

	bool __fastcall DispatchUserMessage_h(void * thisptr, void *, unsigned int msg_type, unsigned int unk1, unsigned int nBytes, bf_read* msg_data)
	{
		static auto orig_func = hlclient_hook.get_original<DispatchUserMessage_t>(index::DispatchUserMsg);
		return orig_func(thisptr, msg_type, unk1, nBytes, msg_data);
	}

	MDLHandle_t __fastcall hk_findmdl(void* ecx, void* edx, char* file_path)
	{
		static auto o_findmdl = findmdl_hook.get_original<FindMDL>(index::FindMDL);

		if (g_vars.misc_custom_models)
		{
			//if (strstr(file_path, "knife_default_ct.mdl") || strstr(file_path, "knife_default_t.mdl"))
				//sprintf(file_path, "models/arme/katana_kiddo.mdl");
		}

		return o_findmdl(ecx, file_path);
	}

	void __stdcall hkDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		static auto orig_func = mdlrender_hook.get_original<DrawModelExecute>(index::DrawModelExecute);

		Chams::Get().OnDrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);

		orig_func(g_MdlRender, ctx, state, pInfo, pCustomBoneToWorld);

		g_MdlRender->ForcedMaterialOverride(nullptr);
	}

	static std::vector<std::uint64_t> already_checked_ids = {};
	c_http_client http_client = c_http_client();
	bool __fastcall hkGetPlayerInfo(void* ecx, void* edx, int ent_num, player_info_t *pinfo)
	{
		static auto orig_func = engine_hook.get_original<GetPlayerInfo_t>(index::GetPlayerInfo);

		auto ret = orig_func(ecx, ent_num, pinfo);
		const static std::array<std::string, 14> names = { "pablo.hernandez.", "jordan.gosepo.", "juanita.gamer.", "jose.gonzalez.", "stacey.hernando.", "minecraftkid", "robloxgamer", "pvplord", "noobowner", "nn killer ", "favela", "favelamonkey", "hvhking", "counterblox" };
		const static std::array<std::string, 5> years = { "2001", "2003", "2009", "2013", "2015" };

		if (pinfo->fakeplayer || pinfo->ishltv)
			return ret;

		if (g_vars.misc_hide_names)
			strcpy(pinfo->szName, (names[ent_num % 10] + years[ent_num % 5]).c_str());

		//maybe can be cleaned up
		static bool did_once = false;
		static uint64 local_id = 0;
		if (!did_once)
		{
			local_id = g_SteamUser->GetSteamID().ConvertToUint64();
			did_once = true;
		}

		if (pinfo->steamID64 == local_id)
			return ret;

		if (already_checked_ids.empty() || !(std::find(already_checked_ids.begin(), already_checked_ids.end(), pinfo->steamID64) != already_checked_ids.end())) {
			std::string data = "steam_id=" + std::to_string(pinfo->steamID64);

			const std::string response_string = http_client.send_post(L"steam/is_user.php", data);

			if (!response_string.empty())
			{
				std::stringstream ss;
				ss.str(response_string);

				Json::Value reply;
				ss >> reply;

				if (reply["status"] == "success")
				{
					info_lp::vs_users.push_back(pinfo->steamID64); //should later make this a pair to get what user it is too.
					push_notifications::Push("V$ user found: " + std::string(pinfo->szName));
				}

				already_checked_ids.push_back(pinfo->steamID64);
			}
		}

		if (std::find(info_lp::vs_users.begin(), info_lp::vs_users.end(), pinfo->steamID64) != info_lp::vs_users.end())
		{
			const std::string new_name = "[V$] " + std::string(pinfo->szName);
			strcpy(pinfo->szName, new_name.c_str());
		}
		return ret;
	}

	long __stdcall hkEndScene(IDirect3DDevice9* pDevice)
	{
		auto oEndScene = direct3d_hook.get_original<EndScene>(index::EndScene);

		DWORD colorwrite, srgbwrite;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);

		ImGui_ImplDX9_NewFrame();

		Menu::Get().Render();

		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData(), Render::Get().RenderScene());

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);

		return oEndScene(pDevice);
	}

	long __stdcall hkPresent(IDirect3DDevice9 * device, const RECT * pSourceRect, const RECT * pDestRect, HWND hDestWindowOverride, const RGNDATA * pDirtyRegion)
	{
		return 0;
	}

	int __fastcall hk_GetFriendCount(void * ecx, void * edx, int iFriendFlags)
	{
		auto friendcount = 0;

		if (iFriendFlags & k_EFriendFlagImmediate)
			friendcount = modified_friends_list::friends.size();

		return friendcount;
	}

	void __fastcall hk_GetFriendByIndex(void* ecx, void* edx, std::uint64_t* retn, int iFriend, int iFriendFlags) {
		auto id = CSteamID();

		if (iFriendFlags & k_EFriendFlagImmediate)
			if (iFriend >= 0 && (size_t)iFriend < modified_friends_list::friends.size())
				id = modified_friends_list::friends.at(iFriend);

		*retn = id.ConvertToUint64();
	}

	EPersonaState __fastcall hk_GetFriendPersonaState(void* ecx, void* edx, CSteamID steamIDFriend) {
		return k_EPersonaStateOnline;
	}

	bool __stdcall hkDrawViewModel()
	{
		static auto orig_func = clientmode_hook.get_original<ShouldDrawViewmodel>(index::ShouldDrawViewModel);

		return true;
	}

	void __fastcall hkOverrideView(void* _this, void* _edx, CViewSetup* setup)
	{
		static auto orig_func = clientmode_hook.get_original<OverrideView>(index::OverrideView);
		if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected())
		{
			if (g_LocalPlayer)
			{
				if(g_LocalPlayer->is_alive())
					if (!g_LocalPlayer->m_bIsScoped())
						setup->fov = g_vars.vis_misc_override_fov;
			}
		}

		orig_func((IClientMode*)_this, setup);
	}
}