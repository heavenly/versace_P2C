#include "bullet_impact.h"
#include "valve_sdk/csgostructs.hpp"
#include "options.hpp"
#include "resolver.h"

void bullet_impact_event::fire_game_event(IGameEvent *event)
{
	if (!g_LocalPlayer || !event)
		return;


	C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt("userid")));
	float x = event->GetFloat("x"), y = event->GetFloat("y"), z = event->GetFloat("z");
	if (g_vars.vis_misc_bullet_beams)
	{
		if (entity != g_LocalPlayer && g_vars.vis_misc_bullet_beams_targets[0] && entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
		{
			v_bullet_info.push_back({ g_GlobalVars->curtime, Vector(x, y, z), 0, entity });
		}
		if (entity != g_LocalPlayer && g_vars.vis_misc_bullet_beams_targets[1] && entity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum())
		{
			v_bullet_info.push_back({ g_GlobalVars->curtime, Vector(x, y, z), 1, entity });
		}
		if (entity == g_LocalPlayer && g_LocalPlayer->is_alive())
		{
			if (g_vars.vis_misc_bullet_beams_targets[2])
			{
				v_bullet_info.push_back({ g_GlobalVars->curtime, Vector(x, y, z), 2, entity });
			}
		}
	}
	if (entity == g_LocalPlayer && g_LocalPlayer->is_alive())
	{
		resolver.on_bullet_impact(Vector(x, y, z));
	}
}

int bullet_impact_event::get_event_debug_id(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void bullet_impact_event::register_self()
{
	g_GameEvents->AddListener(this, "bullet_impact", false);
}

void bullet_impact_event::unregister_self()
{
	g_GameEvents->RemoveListener(this);
}

void bullet_impact_event::paint(void)
{
	if (!g_vars.vis_misc_bullet_beams)
		return;

	if (!g_EngineClient->IsInGame() || !g_LocalPlayer || !g_LocalPlayer->is_alive())
	{
		v_bullet_info.clear();
		return;
	}

	std::vector<bullet_impact_info> &impacts = v_bullet_info;

	if (impacts.empty())
		return;

	auto get_color_beam = [=](int team) -> Color
	{
		int sub_team = team % 3;
		return g_vars.color_bullet_beams[sub_team];
	};

	auto get_color_impact = [=](int team) -> Color
	{
		int sub_team = team % 3;
		return g_vars.color_bullet_impacts[sub_team];
	};

	for (size_t i = 0; i < impacts.size(); i++)
	{
		auto current_impact = impacts.at(i);
		auto current_color = get_color_beam(current_impact.player_type);

		if (g_vars.vis_misc_bullet_beams_type == 0)
		{
			BeamInfo_t beam_info;

			beam_info.m_nType = TE_BEAMPOINTS;
			beam_info.m_pszModelName = "sprites/physbeam.vmt";
			beam_info.m_nModelIndex = -1;
			beam_info.m_flHaloScale = 0.0f;
			beam_info.m_flLife = g_vars.vis_misc_bullet_beams_lifetime;
			beam_info.m_flWidth = g_vars.vis_misc_bullet_beams_size;
			beam_info.m_flEndWidth = g_vars.vis_misc_bullet_beams_size;
			beam_info.m_flFadeLength = 0.0f;
			beam_info.m_flAmplitude = 2.0f;
			beam_info.m_flBrightness = current_color.a();
			beam_info.m_flSpeed = 0.2f;
			beam_info.m_nStartFrame = 0;
			beam_info.m_flFrameRate = 0.f;
			beam_info.m_flRed = current_color.r();
			beam_info.m_flGreen = current_color.g();
			beam_info.m_flBlue = current_color.b();
			beam_info.m_nSegments = 2;
			beam_info.m_bRenderable = true;
			beam_info.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;

			beam_info.m_vecStart = current_impact.player_who_shot->get_eye_pos();
			beam_info.m_vecEnd = current_impact.hit_pos;

			auto beam = g_RenderBeams->CreateBeamPoints(beam_info);

			if (beam)
				g_RenderBeams->DrawBeam(beam);
		}

		if (g_vars.vis_misc_bullet_beams_type == 1)
		{
			auto start = current_impact.player_who_shot->get_eye_pos();
			auto end = current_impact.hit_pos;
			g_DebugOverlay->AddLineOverlay(start, end, current_color.r(), current_color.g(), current_color.b(), true, g_vars.vis_misc_bullet_beams_lifetime);
		}

		auto color = get_color_impact(current_impact.player_type);

		auto pos_1 = -(g_vars.vis_misc_bullet_impacts_size / 2);
		auto pos_2 = abs(pos_1);

		g_DebugOverlay->AddBoxOverlay(current_impact.hit_pos, Vector(pos_1, pos_1, pos_1), Vector(pos_2, pos_2, pos_2), Vector(0, 0, 0), color.r(), color.g(), color.b(), color.a(), g_vars.vis_misc_bullet_impacts_lifetime);

		impacts.erase(impacts.begin() + i);
	}
}