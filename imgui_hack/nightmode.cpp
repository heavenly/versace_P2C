#include "nightmode.h"
#include "options.hpp"
#include "valve_sdk/Misc/Convar.hpp"
#include "helpers/utils.hpp"

void c_nightmode::do_nightmode() {

	static bool performed = false;
	static bool nightmode_last = false;
	static bool asus_last = false;
	static float asus_normal_last = 4.20f;
	static float asus_staticprop_last = 4.20f;
	static float nightmode_normal_last = 4.20f;
	static float nightmode_staticprop_last = 4.20f;


	if (!g_EngineClient->IsConnected() || !g_EngineClient->IsInGame()) {

		if (performed)
			performed = false;
		return;
	}

	auto local_player = g_LocalPlayer;

	if (!local_player)
	{
		if (performed)
			performed = false;
		return;
	}

	if(nightmode_last != g_vars.misc_nightmode)
	{
		nightmode_last = g_vars.misc_nightmode;
		performed = false;
	}

	if(g_vars.misc_nightmode_normal_val != nightmode_normal_last)
	{
		nightmode_normal_last = g_vars.misc_nightmode_normal_val;
		performed = false;
	}

	if(g_vars.misc_nightmode_staticprop_val != nightmode_staticprop_last)
	{
		nightmode_staticprop_last = g_vars.misc_nightmode_staticprop_val;
		performed = false;
	}

	if (asus_last != g_vars.misc_asus)
	{
		asus_last = g_vars.misc_asus;
		performed = false;
	}

	if (g_vars.misc_asus_normal_val != asus_normal_last)
	{
		asus_normal_last = g_vars.misc_asus_normal_val;
		performed = false;
	}

	if (g_vars.misc_asus_staticprop_val != asus_staticprop_last)
	{
		asus_staticprop_last = g_vars.misc_asus_staticprop_val;
		performed = false;
	}
	
	if (!performed) {
		static ConVar* draw_specific_static_prop = g_CVar->FindVar("r_DrawSpecificStaticProp");
		draw_specific_static_prop->RemoveFlags(FCVAR_CHEAT);

		draw_specific_static_prop->SetValue(!g_vars.misc_nightmode);

		if(g_vars.misc_nightmode)
			Utils::set_sky("sky_csgo_night02");
		else
			Utils::set_sky("vertigoblue_hdr");

		for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i)) {

			IMaterial *material = g_MatSystem->GetMaterial(i);

			if (!material)
				continue;

			if (strstr(material->GetTextureGroupName(), "World"))
			{
				if (g_vars.misc_nightmode)
					material->ColorModulate(g_vars.misc_nightmode_normal_val, g_vars.misc_nightmode_normal_val, g_vars.misc_nightmode_normal_val);
				else
					material->ColorModulate(1, 1, 1);

				if (g_vars.misc_asus)
					material->AlphaModulate(g_vars.misc_asus_normal_val);
				else
					material->AlphaModulate(1);
			}
			else if (strstr(material->GetTextureGroupName(), "StaticProp"))
			{
				if (g_vars.misc_nightmode)
					material->ColorModulate(g_vars.misc_nightmode_staticprop_val, g_vars.misc_nightmode_staticprop_val, g_vars.misc_nightmode_staticprop_val);
				else
					material->ColorModulate(1, 1, 1);

				if (g_vars.misc_asus)
					material->AlphaModulate(g_vars.misc_asus_staticprop_val);
				else
					material->AlphaModulate(1);
			}
		} 
		performed = true;
	}
}
