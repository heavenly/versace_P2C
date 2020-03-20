#include "lists.h"

namespace menu_lists
{
	v_string hitboxes =
	{
	"head",
	"neck",
	"pelvis",
	"stomach",
	"lower chest",
	"chest",
	"upper chest",
	"right thigh",
	"left thigh",
	"right calf",
	"left calf",
	"right foot",
	"left foot",
	"right hand",
	"left hand",
	"right upper arm",
	"right forearm",
	"left upper arm",
	"left forearm"
	};
	v_string skyboxes =
	{
	"cs__baggage_skybox_",
	"cs_tibet",
	"clearsky",
	"clearsky_hdr",
	"embassy",
	"italy",
	"jungle",
	"nukeblank",
	"office",
	"sky_cs15_daylight01_hdr",
	"sky_cs15_daylight02_hdr",
	"sky_cs15_daylight03_hdr",
	"sky_cs15_daylight04_hdr",
	"sky_csgo_cloudy01",
	"sky_csgo_night02",
	"sky_csgo_night02b",
	"sky_csgo_night_flat",
	"sky_day02_05_hdr",
	"sky_day02_05",
	"sky_dust",
	"sky_l4d_rural02_ldr",
	"vertigo_hdr",
	"vertigoblue_hdr",
	"vertigo",
	"vietnam",
	"amethyst",
	"sky_descent",
	"clear_night_sky",
	"otherworld",
	"cloudynight",
	"dreamyocean",
	"grimmnight",
	"sky051",
	"sky081",
	"sky091",
	"sky561"
	};
	v_string backtrack_chams_styles =
	{
	"all ticks",
	"last tick"
	};
	v_string glow_styles =
	{
		"Outline outer", "Cover", "Outline inner"
	};
	v_string autostrafe_types =
	{
		"old", "old inverse", "old optimizer", "new"
	};
	v_string resolver_type =
	{
		"lby", "BYOR"
	};
	v_string hitboxes_shortened =
	{
		"generic",
		"head",
		"chest",
		"stomach",
		"left arm",
		"right arm",
		"left leg",
		"right leg",
		"gear"
	};

	v_string origin_box_styles =
	{
		"always",
		"dormant only"
	};

	v_string distance_styles =
	{
		"feet",
		"meters"
	};

	std::vector<std::pair<std::string, int>> weapon_list =
	{
		{"WEAPON_DEAGLE" , 1 },
	{"WEAPON_ELITE", 2},
	{"WEAPON_FIVESEVEN", 3},
	{"WEAPON_GLOCK", 4},
	{"WEAPON_AK47", 7},
	{"WEAPON_AUG", 8},
	{"WEAPON_AWP", 9},
	{"WEAPON_FAMAS", 10},
	{"WEAPON_G3SG1", 11},
	{"WEAPON_GALILAR", 13},
	{"WEAPON_M249", 14},
	{"WEAPON_M4A1", 16},
	{"WEAPON_MAC10", 17},
	{"WEAPON_P90", 19},
	{"WEAPON_MP5", 23},
	{"WEAPON_UMP45", 24},
	{"WEAPON_XM1014", 25},
	{"WEAPON_BIZON", 26},
	{"WEAPON_MAG7", 27},
	{"WEAPON_NEGEV", 28},
	{"WEAPON_SAWEDOFF", 29},
	{"WEAPON_TEC9", 30},
	{"WEAPON_TASER", 31},
	{"WEAPON_HKP2000", 32},
	{"WEAPON_MP7", 33},
	{"WEAPON_MP9", 34},
	{"WEAPON_NOVA", 35},
	{"WEAPON_P250", 36},
	{"WEAPON_SCAR20", 38},
	{"WEAPON_SG556", 39},
	{"WEAPON_SSG08", 40},
	{"WEAPON_KNIFE", 42},
	{"WEAPON_FLASHBANG", 43},
	{"WEAPON_HEGRENADE", 44},
	{"WEAPON_SMOKEGRENADE", 45},
	{"WEAPON_MOLOTOV", 46},
	{"WEAPON_DECOY", 47},
	{"WEAPON_INCGRENADE", 48},
	{"WEAPON_C4", 49},
	{"WEAPON_KNIFE_T", 59},
	{"WEAPON_M4A1_SILENCER", 60},
	{"WEAPON_USP_SILENCER", 61},
	{"WEAPON_CZ75A", 63},
	{"WEAPON_REVOLVER", 64},
	};
	v_string weapon_list_names =
	{
		"WEAPON_DEAGLE" ,
	"WEAPON_ELITE" ,
	"WEAPON_FIVESEVEN",
	"WEAPON_GLOCK",
	"WEAPON_AK47",
	"WEAPON_AUG",
	"WEAPON_AWP",
	"WEAPON_FAMAS",
	"WEAPON_G3SG1",
	"WEAPON_GALILAR",
	"WEAPON_M249",
	"WEAPON_M4A1",
	"WEAPON_MAC10",
	"WEAPON_P90",
	"WEAPON_MP5",
	"WEAPON_UMP45",
	"WEAPON_XM1014",
	"WEAPON_BIZON",
	"WEAPON_MAG7",
	"WEAPON_NEGEV",
	"WEAPON_SAWEDOFF",
	"WEAPON_TEC9",
	"WEAPON_TASER",
	"WEAPON_HKP2000",
	"WEAPON_MP7",
	"WEAPON_MP9",
	"WEAPON_NOVA",
	"WEAPON_P250",
	"WEAPON_SCAR20",
	"WEAPON_SG556",
	"WEAPON_SSG08",
	"WEAPON_KNIFE",
	"WEAPON_FLASHBANG",
	"WEAPON_HEGRENADE",
	"WEAPON_SMOKEGRENADE",
	"WEAPON_MOLOTOV",
	"WEAPON_DECOY",
	"WEAPON_INCGRENADE",
	"WEAPON_C4",
	"WEAPON_KNIFE_T",
	"WEAPON_M4A1_SILENCER",
	"WEAPON_USP_SILENCER",
	"WEAPON_CZ75A",
	"WEAPON_REVOLVER",
	};
	v_string knife_names =
	{
		"WEAPON_KNIFE_CT",
	"WEAPON_KNIFE_T",
	"WEAPON_BAYONET",
	"WEAPON_KNIFE_FLIP",
	"WEAPON_KNIFE_GUT",
	"WEAPON_KNIFE_KARAMBIT",
	"WEAPON_KNIFE_M9_BAYONET",
	"WEAPON_KNIFE_TACTICAL",
	"WEAPON_KNIFE_FALCHION",
	"WEAPON_KNIFE_SURVIVAL_BOWIE",
	"WEAPON_KNIFE_BUTTERFLY",
	"WEAPON_KNIFE_PUSH",
	"WEAPON_KNIFE_GYPSY",
	"WEAPON_KNIFE_STILETTO",
	"WEAPON_KNIFE_URSUS",
	"WEAPON_KNIFE_WIDOWMAKER",
	};
	std::vector<std::pair<std::string, int>> knife_list =
	{
	{"WEAPON_KNIFE_CT", 42},
	{"WEAPON_KNIFE_T", 59},
	{"WEAPON_BAYONET", 500},
	{"WEAPON_KNIFE_FLIP", 505},
	{"WEAPON_KNIFE_GUT", 506},
	{"WEAPON_KNIFE_KARAMBIT", 507},
	{"WEAPON_KNIFE_M9_BAYONET", 508},
	{"WEAPON_KNIFE_TACTICAL", 509},
	{"WEAPON_KNIFE_FALCHION", 512},
	{"WEAPON_KNIFE_SURVIVAL_BOWIE", 514},
	{"WEAPON_KNIFE_BUTTERFLY", 515},
	{"WEAPON_KNIFE_PUSH", 516},
	{"WEAPON_KNIFE_GYPSY", 520},
	{"WEAPON_KNIFE_STILETTO", 522},
	{"WEAPON_KNIFE_URSUS", 519},
	{"WEAPON_KNIFE_WIDOWMAKER", 523},
	};

	v_string hitmarker_sounds = 
	{
		"P2C",
		"kaching"
	};

	v_string resolve_base_angles =
	{
		"lby",
		"moving lby",
		"eye angles",
		"at target"
	};

	v_string bullet_beam_types =
	{
		"glow beams",
		"line beams"
	};

	v_string execution_loc_types =
	{
		"none (disabled)",
		"create move",
		"frame stage notify",
		"paint traverse"
	};
};