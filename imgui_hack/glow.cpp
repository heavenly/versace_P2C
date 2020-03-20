#include "glow.h"

#include "options.hpp"
#include "visuals_pt.h"

std::vector<std::pair<int, int>> custom_glow_entities;

void Glow::render_glow()
{
	static bool glow_cleared = false;
	if (!g_vars.glow_enabled)
	{
		if (!glow_cleared)
		{
			Glow::clear_glow();
			glow_cleared = true;
		}
		return;
	}

	if (g_vars.glow_on_key > 0 && !GetAsyncKeyState(g_vars.glow_on_key))
	{
		if (!glow_cleared)
		{
			Glow::clear_glow();
			glow_cleared = true;
		}
		return;
	}

	glow_cleared = false;

	for (auto i = 0; i < g_GlowObjManager->m_GlowObjectDefinitions.Count(); i++)
	{
		auto& glow_object = g_GlowObjManager->m_GlowObjectDefinitions[i];
		auto entity = glow_object.m_pEntity;

		if (glow_object.IsUnused())
			continue;

		if (!entity)
			continue;

		auto class_id = entity->GetClientClass()->m_ClassID;
		Color color;

		switch (class_id)
		{
		case ClassId_CCSPlayer:
		{
			auto entity_as_player = (C_BasePlayer*)entity;
			bool is_enemy = entity_as_player->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
			bool is_team = entity_as_player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum();

			if (!g_vars.glow_players || !entity_as_player->is_alive() || entity->is_dormant())
				continue;

			glow_object.m_nGlowStyle = 0;

			bool vis = entity_as_player->is_visible();
			Color cur_color = (is_enemy ? (vis ? g_vars.color_glow[1][0] : g_vars.color_glow[1][1]) : (vis ? g_vars.color_glow[0][0] : g_vars.color_glow[0][1]));

			color = cur_color;
			break;
		}

		case ClassId_CBaseAnimating:

			if (!g_vars.glow_others)
				continue;

			glow_object.m_nGlowStyle = g_vars.glow_others_style;

			color = Color::Blue;

			break;

		case ClassId_CPlantedC4:

			if (!g_vars.glow_others)
				continue;

			glow_object.m_nGlowStyle = g_vars.glow_others_style;

			color = g_vars.color_glow_c4;

			break;

		default:
		{
			auto entity_as_weapon = (C_BaseCombatWeapon*)entity;
			if (entity_as_weapon->IsWeapon())
			{
				if (!g_vars.glow_others)
					continue;

				glow_object.m_nGlowStyle = g_vars.glow_others_style;

				color = g_vars.color_glow_others_weapons;
			}
			break;
		}
	}

		glow_object.m_flRed = color.r() / 255.0f;
		glow_object.m_flGreen = color.g() / 255.0f;
		glow_object.m_flBlue = color.b() / 255.0f;
		glow_object.m_flAlpha = color.a() / 255.0f;
		glow_object.m_bRenderWhenOccluded = true;
		glow_object.m_bRenderWhenUnoccluded = false;
	}
}

void Glow::add_to_glow_mgr(ClientFrameStage_t stage)
{
	// Skip reserved slots that are guaranteed to be managed by the engine.
	for (int i = 64; i < g_EntityList->GetHighestEntityIndex(); i++)
	{
		C_BaseEntity* entity = (C_BaseEntity*)g_EntityList->GetClientEntity(i);

		// Register custom entities into the glow object definitions array.
		if (g_EngineClient->IsInGame() && entity && entity->GetClientClass()->m_ClassID == ClassId_CBaseAnimating)
		{
			if (!g_GlowObjManager->HasGlowEffect(entity))
			{
				int array_index = g_GlowObjManager->RegisterGlowObject(entity, Vector(0, 0, 0), 0, false, false, -1);

				if (array_index != -1)
					custom_glow_entities.emplace_back(i, array_index);
			}
		}
		else
		{
			// Remove any entities that no longer exist.
			auto iterator = std::find_if(custom_glow_entities.begin(), custom_glow_entities.end(),
				[&](const std::pair<int, int>& p) {
					return p.first == i;
				}
			);

			if (iterator != custom_glow_entities.end())
			{
				g_GlowObjManager->UnregisterGlowObject(iterator->second);
				custom_glow_entities.erase(iterator);
			}
		}
	}
}

void Glow::clear_glow()
{
	for (auto i = 0; i < g_GlowObjManager->m_GlowObjectDefinitions.Count(); i++)
	{
		auto& glow_object = g_GlowObjManager->m_GlowObjectDefinitions[i];
		auto entity = reinterpret_cast<C_BasePlayer*>(glow_object.m_pEntity);

		if (glow_object.IsUnused())
			continue;

		if (!entity || entity->is_dormant())
			continue;

		glow_object.m_bRenderWhenOccluded = false;
		glow_object.m_bRenderWhenUnoccluded = false;
		glow_object.m_flAlpha = 0.0f;
	}
}
