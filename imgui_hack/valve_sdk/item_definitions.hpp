#pragma once
#include <map>
#include <vector>
#include "csgostructs.hpp"

struct hud_weapons_t
{
	std::int32_t* get_weapon_count()
	{
		return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
	}
};

static auto is_knife(const int i) -> bool
{
	return (i >= WEAPON_BAYONET && i < STUDDED_HYDRA_GLOVES) || i == WEAPON_KNIFE_T || i == WEAPON_KNIFE;
}

struct weapon_info
{
	constexpr weapon_info(const char* model, const char* icon = nullptr) :
		model(model),
		icon(icon)
	{}
	const char* model;
	const char* icon;
};
struct weapon_name
{
	constexpr weapon_name(const int definition_index, const char* name) :
		definition_index(definition_index),
		name(name)
	{}
	int definition_index = 0;
	const char* name = nullptr;
};
extern const std::map<size_t, weapon_info> k_weapon_info;
extern const std::vector<weapon_name> k_knife_names;
extern const std::vector<weapon_name> k_glove_names;
extern const std::vector<weapon_name> k_weapon_names_s;
