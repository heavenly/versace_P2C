#include "grenades.h"
#include "valve_sdk/csgostructs.hpp"
#include "options.hpp"

void grenade_detonate_event::fire_game_event(IGameEvent *event)
{
	if (!g_LocalPlayer || !event)
		return;

	if (g_vars.vis_misc_grenade_circle)
	{
			float x = event->GetFloat("x"), y = event->GetFloat("y"), z = event->GetFloat("z");
			//0 = smoke
			int type = 0;
			if(!strcmp(event->GetName(), "smokegrenade_detonate"))
			{
				type = 0;
			}
			else if (!strcmp(event->GetName(), "molotov_detonate"))
			{
				type = 1;
			}
			else if (!strcmp(event->GetName(), "decoy_started"))
			{
				type = 2;
			}
			v_grenade_info.push_back({ type, Vector(x, y, z) });
	}
}

int grenade_detonate_event::get_event_debug_id(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void grenade_detonate_event::register_self()
{
	g_GameEvents->AddListener(this, "smokegrenade_detonate", false);
	g_GameEvents->AddListener(this, "molotov_detonate", false);
	g_GameEvents->AddListener(this, "decoy_started", false);
}

void grenade_detonate_event::unregister_self()
{
	g_GameEvents->RemoveListener(this);
}

void grenade_detonate_event::paint(void)
{
	if (!g_vars.vis_misc_grenade_circle)
		return;

	if (!g_EngineClient->IsInGame() || !g_LocalPlayer)
	{
		v_grenade_info.clear();
		return;
	}

	auto get_grenade_life = [=](int type) -> int
	{
		switch (type % 3)
		{
		case 0: return 19;
		case 1: return 7;
		case 2: return 15;
		}
		return{};
	};

	auto get_grenade_color = [=](int type) -> Color
	{
		switch (type % 3)
		{
		case 0: return g_vars.color_beam_smoke;
		case 1: return g_vars.color_beam_molotov;
		case 2: return g_vars.color_beam_decoy;
		}
		return {};
	};

	auto get_grenade_size = [=](int type) -> int
	{
		switch (type % 3)
		{
		case 0: return 430;
		case 1: return 400;
		case 2: return 15;
		}
		return {};
	};

	std::vector<grenade> &grenade_list = v_grenade_info;

	if (grenade_list.empty())
		return;

	for (size_t i = 0; i < grenade_list.size(); i++)
	{
		auto current_grenade = grenade_list.at(i);
		auto current_color = get_grenade_color(current_grenade.type);

		BeamInfo_t beam_info;
		beam_info.m_nType = TE_BEAMRINGPOINT;
		beam_info.m_pszModelName = "sprites/physbeam.vmt";
		beam_info.m_nModelIndex = g_MdlInfo->GetModelIndex("sprites/physbeam.vmt");
		beam_info.m_pszHaloName = "sprites/physbeam.vmt";
		beam_info.m_nHaloIndex = g_MdlInfo->GetModelIndex("sprites/physbeam.vmt");
		beam_info.m_flHaloScale = 5;
		beam_info.m_flLife = get_grenade_life(current_grenade.type);
		beam_info.m_flWidth = 10;
		beam_info.m_flFadeLength = .3f;
		beam_info.m_flAmplitude = 0.f;
		beam_info.m_flBrightness = 255;
		beam_info.m_flSpeed = 0;
		beam_info.m_nStartFrame = 0;
		beam_info.m_flFrameRate = 1;
		beam_info.m_flRed = current_color.r();
		beam_info.m_flGreen = current_color.g();
		beam_info.m_flBlue = current_color.b();
		beam_info.m_nSegments = 1;
		beam_info.m_bRenderable = true;
		beam_info.m_nFlags = 0;
		beam_info.m_vecCenter = current_grenade.location;
		beam_info.m_flStartRadius = 1;
		beam_info.m_flEndRadius = get_grenade_size(current_grenade.type);

		const auto beam = g_RenderBeams->CreateBeamRingPoint(beam_info);
		if (!beam)
			return;

		g_RenderBeams->DrawBeam(beam);

		grenade_list.erase(grenade_list.begin() + i);
	}
}