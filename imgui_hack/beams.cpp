#include "beams.h"
#include "options.hpp"
#include "far_esp.h"
void c_beams::draw_rings(C_BasePlayer * player)
{
	if (!g_vars.vis_misc_footstep_beams)
		return;

	if (!player)
		return;

	if (!player->is_alive() || (player->is_dormant() && far_esp::Get().sound_players[player->EntIndex()].dormant))
		return;

	if (g_GlobalVars->curtime - last_draw_time[player->EntIndex()] < .4 && !player->is_dormant())
		return;

	//if player is dormant ignore this check
	if (player->m_vecOrigin() == last_origin[player->EntIndex()] && !player->is_dormant())
		return;

	const bool local = player == g_LocalPlayer;
	const bool enemy = player->m_iTeamNum() != g_LocalPlayer->m_iTeamNum() && !local;
	const bool team = player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum() && !local;

	if (!g_vars.esp_targets[0] && team)
		return;

	if (!g_vars.esp_targets[1] && enemy)
		return;

	if (local && !g_vars.esp_targets[2])
		return;

	const auto origin = player->is_dormant() ? far_esp::Get().sound_players[player->EntIndex()].origin : player->m_vecOrigin();
	last_origin[player->EntIndex()] = origin;
	last_draw_time[player->EntIndex()] = g_GlobalVars->curtime;

	int team_clr_i = 0;

	if (team)
	{
		team_clr_i = 0;
	}
	else if (enemy)
	{
		team_clr_i = 1;
	}
	else if (local)
	{
		team_clr_i = 2;
	}
	float red = g_vars.color_footstep_beams[team_clr_i].r();
	float green = g_vars.color_footstep_beams[team_clr_i].g();
	float blue = g_vars.color_footstep_beams[team_clr_i].b();

	BeamInfo_t beam_info;
	beam_info.m_nType = TE_BEAMRINGPOINT;
	beam_info.m_pszModelName = "sprites/physbeam.vmt";
	beam_info.m_nModelIndex = g_MdlInfo->GetModelIndex("sprites/physbeam.vmt");
	beam_info.m_pszHaloName = "sprites/physbeam.vmt";
	beam_info.m_nHaloIndex = g_MdlInfo->GetModelIndex("sprites/physbeam.vmt");
	beam_info.m_flHaloScale = 5;
	beam_info.m_flLife = 1.f;
	beam_info.m_flWidth = 10;
	beam_info.m_flFadeLength = .3f;
	beam_info.m_flAmplitude = 0.f;
	beam_info.m_flBrightness = 255;
	beam_info.m_flSpeed = 0;
	beam_info.m_nStartFrame = 0;
	beam_info.m_flFrameRate = 1;
	beam_info.m_flRed = red;
	beam_info.m_flGreen = green;
	beam_info.m_flBlue = blue;
	beam_info.m_nSegments = 1;
	beam_info.m_bRenderable = true;
	beam_info.m_nFlags = 0;
	beam_info.m_vecCenter = origin + Vector(0, 0, 2);
	beam_info.m_flStartRadius = 1;
	beam_info.m_flEndRadius = 200;

	if (const auto beam = g_RenderBeams->CreateBeamRingPoint(beam_info); beam)
	{
		g_RenderBeams->DrawBeam(beam);
	}
}
