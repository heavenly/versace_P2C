#include "skinchanger.h"
#include "options.hpp"
#include "valve_sdk/item_definitions.hpp"
c_skinchanger skinchanger = c_skinchanger();


void c_skinchanger::do_skin_change()
{
	if (!g_LocalPlayer)
		return;

	if (!g_vars.skinchanger_enabled)
		return;

	for (int i = 0; i < g_EntityList->GetHighestEntityIndex(); i++)
	{
		C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*)g_EntityList->GetClientEntity(i);

		if (!weapon)
			continue;

		if (!weapon->IsWeapon())
			continue;

		if (weapon->m_hOwnerEntity() != g_LocalPlayer)
			continue;

		const auto item_def = weapon->m_iItemDefinitionIndex();

		/*
		415 - Doppler (Ruby)
		416 - Doppler (Sapphire)
		417 - Doppler (Blackpearl)
		*/

		const skin_info sk_info = info[item_def];

		weapon->m_iItemIDHigh() = -1;
		weapon->GetAccountID() = weapon->m_OriginalOwnerXuidLow();
		weapon->m_nFallbackPaintKit() = sk_info.skin_id;
		weapon->m_flFallbackWear() = sk_info.wear;
		weapon->m_nFallbackSeed() = sk_info.seed;
	}
}

const weapon_info* get_weapon_info(int defindex)
{
	const static std::map<int, weapon_info> info =
	{
		{WEAPON_KNIFE,{"models/weapons/v_knife_default_ct.mdl", "knife_default_ct"}},
		{WEAPON_KNIFE_T,{"models/weapons/v_knife_default_t.mdl", "knife_t"}},
		{WEAPON_BAYONET, {"models/weapons/v_knife_bayonet.mdl", "bayonet"}},
		{WEAPON_KNIFE_FLIP, {"models/weapons/v_knife_flip.mdl", "knife_flip"}},
		{WEAPON_KNIFE_GUT, {"models/weapons/v_knife_gut.mdl", "knife_gut"}},
		{WEAPON_KNIFE_KARAMBIT, {"models/weapons/v_knife_karam.mdl", "knife_karambit"}},
		{WEAPON_KNIFE_M9_BAYONET, {"models/weapons/v_knife_m9_bay.mdl", "knife_m9_bayonet"}},
		{WEAPON_KNIFE_TACTICAL, {"models/weapons/v_knife_tactical.mdl", "knife_tactical"}},
		{WEAPON_KNIFE_FALCHION, {"models/weapons/v_knife_falchion_advanced.mdl", "knife_falchion"}},
		{WEAPON_KNIFE_SURVIVAL_BOWIE, {"models/weapons/v_knife_survival_bowie.mdl", "knife_survival_bowie"}},
		{WEAPON_KNIFE_BUTTERFLY, {"models/weapons/v_knife_butterfly.mdl", "knife_butterfly"}},
		{WEAPON_KNIFE_PUSH, {"models/weapons/v_knife_push.mdl", "knife_push"}},
		{WEAPON_KNIFE_URSUS,{"models/weapons/v_knife_ursus.mdl", "knife_ursus"}},
		{WEAPON_KNIFE_STILETTO,{"models/weapons/v_knife_stiletto.mdl", "knife_stiletto"}},
		{WEAPON_KNIFE_WIDOWMAKER,{"models/weapons/v_knife_widowmaker.mdl", "knife_widowmaker"}},
		{STUDDED_BLOODHOUND_GLOVES,{"models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"}},
		{T_GLOVES,{"models/weapons/v_models/arms/glove_fingerless/v_glove_fingerless.mdl"}},
		{CT_GLOVES,{"models/weapons/v_models/arms/glove_hardknuckle/v_glove_hardknuckle.mdl"}},
		{SPORTY_GLOVES,{"models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"}},
		{SLICK_GLOVES,{"models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"}},
		{LEATHER_HANDWRAPS,{"models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"}},
		{MOTORCYCLE_GLOVES,{"models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"}},
		{SPECIALIST_GLOVES,{"models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"}},
		{STUDDED_HYDRA_GLOVES,{"models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_hydra.mdl"}}
	};

	const auto entry = info.find(defindex);
	return entry == end(info) ? nullptr : &entry->second;
}
