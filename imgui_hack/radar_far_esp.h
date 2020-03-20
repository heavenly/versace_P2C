#pragma once
#include <array>
#include "valve_sdk/math/Vector.hpp"
#include "valve_sdk/csgostructs.hpp"

struct radar_player_t {
	Vector origin; //0x0000
	Vector viewangles; //0x000C
	char pad_0018[56]; //0x0018
	uint32_t health; //0x0050
	char name[32]; //0x0054
	char pad_00D4[117]; //0x00D4
	uint8_t visible; //0x00E9
}; //credits to sase08 @ https://www.unknowncheats.me/forum/counterstrike-global-offensive/360185-radar-esp.html

struct player_radar_logs
{
	Vector origin;
	uint8_t last_visible_status;
	long long last_spotted_time;
	bool is_valid_log;
};

class c_radar_far_esp
{
	static const int EXPIRY_TIME = 3000;
public:
	static std::array<player_radar_logs, 65> radar_logs;

	static void cache_players();
};