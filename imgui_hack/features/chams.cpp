#include "chams.hpp"
#include <fstream>

#include "../valve_sdk/csgostructs.hpp"
#include "../options.hpp"
#include "../hooks.hpp"
#include "../helpers/input.hpp"
#include "../backtrack.h"

Chams::Chams()
{
	std::ofstream("csgo\\materials\\simple_regular.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
	std::ofstream("csgo\\materials\\simple_ignorez.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
	std::ofstream("csgo\\materials\\simple_flat.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
	std::ofstream("csgo\\materials\\simple_flat_ignorez.vmt") << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";

	std::ofstream("csgo\\materials\\reflective.vmt") << R"#("VertexLitGeneric" {
      "$basetexture" "vgui/white_additive"
      "$ignorez" "0"
      "$envmap" "env_cubemap"
      "$normalmapalphaenvmapmask" "1"
      "$envmapcontrast"  "1"
      "$nofog" "1"
      "$model" "1"
      "$nocull" "0"
      "$selfillum" "1"
      "$halflambert" "1"
      "$znearer" "0"
      "$flat" "1" 
}
)#";

	std::ofstream("csgo\\materials\\reflective_ignorez.vmt") << R"#("VertexLitGeneric" {
      "$basetexture" "vgui/white_additive"
      "$ignorez" "1"
      "$envmap" "env_cubemap"
      "$normalmapalphaenvmapmask" "1"
      "$envmapcontrast"  "1"
      "$nofog" "1"
      "$model" "1"
      "$nocull" "0"
      "$selfillum" "1"
      "$halflambert" "1"
      "$znearer" "0"
      "$flat" "1" 
}
)#";

	// Original shader: SphericalEnvMap
	//"$envmapsphere" 1
	std::ofstream("csgo\\materials\\palace.vmt") << R"#("UnlitGeneric" {
    "$envmap" "dev/dev_envmap"
    "$basetexture"       "dev/screenhighlight_pulse"
    "$multiply"          "1"
    "proxies"    {
        "texturescroll"    {
            "texturescrollvar"   "$basetexturetransform"
            "texturescrollrate"  "0.6"
            "texturescrollangle" "90"
        }
    }
    }
    )#";

	materialRegular = g_MatSystem->FindMaterial("simple_regular", TEXTURE_GROUP_MODEL);
	materialRegularIgnoreZ = g_MatSystem->FindMaterial("simple_ignorez", TEXTURE_GROUP_MODEL);
	materialFlatIgnoreZ = g_MatSystem->FindMaterial("simple_flat_ignorez", TEXTURE_GROUP_MODEL);
	materialFlat = g_MatSystem->FindMaterial("simple_flat", TEXTURE_GROUP_MODEL);
	materialReflective = g_MatSystem->FindMaterial("reflective", TEXTURE_GROUP_MODEL);
	materialReflectiveIgnoreZ = g_MatSystem->FindMaterial("reflective_ignorez", TEXTURE_GROUP_MODEL);
	materialPulse = g_MatSystem->FindMaterial("palace", TEXTURE_GROUP_MODEL);
}

Chams::~Chams()
{
	std::remove("csgo\\materials\\simple_regular.vmt");
	std::remove("csgo\\materials\\simple_ignorez.vmt");
	std::remove("csgo\\materials\\simple_flat.vmt");
	std::remove("csgo\\materials\\simple_flat_ignorez.vmt");
	std::remove("csgo\\materials\\reflective.vmt");
	std::remove("csgo\\materials\\reflective_ignorez.vmt");
	std::remove("csgo\\materials\\palace.vmt");
}

void Chams::override_material(bool ignoreZ, bool flat, bool wireframe, bool glass, bool reflective, bool pulse, const Color& rgba)
{
	IMaterial* material = nullptr;

	if (flat)
	{
		if (ignoreZ)
			material = materialFlatIgnoreZ;
		else
			material = materialFlat;
	}

	if (!flat && !glass)
	{
		if (ignoreZ)
			material = materialRegularIgnoreZ;
		else
			material = materialRegular;
	}

	if (glass) {
		material = materialFlat;
		material->AlphaModulate(0.45f);
	}
	else
		material->AlphaModulate(rgba.a() / 255.0f);

	if (reflective)
	{
		if (ignoreZ)
			material = materialReflectiveIgnoreZ;
		else
			material = materialReflective;
	}

	if (pulse)
		material = materialPulse;

	material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
	material->ColorModulate(
		rgba.r() / 255.0f,
		rgba.g() / 255.0f,
		rgba.b() / 255.0f);

	g_MdlRender->ForcedMaterialOverride(material);
}

void Chams::OnDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t &state, const ModelRenderInfo_t &info, matrix3x4_t *matrix)
{
	static auto fnDME = Hooks::mdlrender_hook.get_original<Hooks::DrawModelExecute>(index::DrawModelExecute);

	const auto mdl = info.pModel;

	if (!mdl)
		return;

	const bool is_arm = strstr(mdl->szName, "arms") != nullptr;
	const bool is_player = strstr(mdl->szName, "models/player") != nullptr;
	const bool is_sleeve = strstr(mdl->szName, "sleeve") != nullptr;
	//const bool is_weapon = strstr(mdl->szName, "weapons/v_")  != nullptr;

	if (!is_player || !g_vars.chams_enabled)
		return;

	auto ent = C_BasePlayer::GetPlayerByIndex(info.entity_index);

	if (!g_LocalPlayer)
		return;

	if (!ent)
		return;

	if (!ent->is_alive())
		return;

	const auto enemy = ent->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
	const auto team = ent->m_iTeamNum() == g_LocalPlayer->m_iTeamNum();
	const auto local = ent == g_LocalPlayer;

	if (team && !local && !g_vars.chams_targets[0])
		return;

	if (enemy && !local && !g_vars.chams_targets[1])
		return;

	if (local && !g_vars.chams_targets[2])
		return;

	const auto clr_visible = enemy ? g_vars.color_chams[1][0] : g_vars.color_chams[0][0];
	const auto clr_occluded = enemy ? g_vars.color_chams[1][1] : g_vars.color_chams[0][1];

	const auto clr_backtrack_visible = enemy ? g_vars.color_chams_backtrack[1][0] : g_vars.color_chams_backtrack[0][0];
	const auto clr_backtrack_occluded = enemy ? g_vars.color_chams_backtrack[1][1] : g_vars.color_chams_backtrack[0][1];

	const auto local_clr = g_vars.color_chams[2][0];

	//really bad looking code wtf
	if (g_vars.backtrack_chams && !backtrack.entity_records[ent->EntIndex()].empty())
	{
		if (g_vars.chams_ignorez) {
			const auto draw_record = [=](backtrack_record& record) -> void
			{
				override_material(true, g_vars.chams_flat, g_vars.chams_wireframe, g_vars.chams_glass, g_vars.chams_reflective, g_vars.chams_pulse, local ? local_clr : clr_backtrack_occluded);
				fnDME(g_MdlRender, ctx, state, info, record.ent_matrix.matrix);
				override_material(false, g_vars.chams_flat, g_vars.chams_wireframe, g_vars.chams_glass, g_vars.chams_reflective, g_vars.chams_pulse, local ? local_clr : clr_backtrack_visible);
				fnDME(g_MdlRender, ctx, state, info, record.ent_matrix.matrix);
			};
			
			if (g_vars.backtrack_chams_style == 0)
			{
				for (auto& record : backtrack.entity_records[ent->EntIndex()])
				{
					if ((ent->m_vecOrigin() - record.origin).LengthSqr() > 4096 && g_vars.backtrack_chams_lc_break)
						continue;

					draw_record(record);
				}
			}
			else
			{
				auto record = backtrack.entity_records[ent->EntIndex()].front();
				draw_record(record);
			}
		}
		else {
			override_material(false, g_vars.chams_flat, g_vars.chams_wireframe, g_vars.chams_glass, g_vars.chams_reflective, g_vars.chams_pulse, local ? local_clr : clr_backtrack_visible);
			if (g_vars.backtrack_chams_style == 0)
			{
				for (auto& record : backtrack.entity_records[ent->EntIndex()])
				{
					//skip first tick
					if (record.tick == backtrack.entity_records[ent->EntIndex()].back().tick)
						continue;

					if ((ent->m_vecOrigin() - record.origin).LengthSqr() > 4096 && g_vars.backtrack_chams_lc_break)
						continue;

					fnDME(g_MdlRender, ctx, state, info, record.ent_matrix.matrix);
				}
			}
			else
			{
				auto record = backtrack.entity_records[ent->EntIndex()].front();
				fnDME(g_MdlRender, ctx, state, info, record.ent_matrix.matrix);
			}
		}
	}

	if (g_vars.chams_ignorez) 
	{
		override_material(true, g_vars.chams_flat, g_vars.chams_wireframe, g_vars.chams_glass, g_vars.chams_reflective, g_vars.chams_pulse, local ? local_clr : clr_occluded);
		fnDME(g_MdlRender, ctx, state, info, matrix);
	}
	override_material(false, g_vars.chams_flat, g_vars.chams_wireframe, g_vars.chams_glass, g_vars.chams_reflective, g_vars.chams_pulse, local ? local_clr : clr_visible);
}
