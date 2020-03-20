#pragma once
#include <array>
struct player_info
{
	int choked_ticks;
	int angle_delta;
	float moving_lby;
};

class c_anticheat
{
public:
	std::array<player_info, 65> players;
	void calculate_choked_ticks();
	void calculate_angle_deltas();
	void do_createmove_log();
	void fix_players();
	void do_lby_pred();
	std::array<float, 65> best_time;
private:
	std::array<float, 65> last_simtime;
	std::array<float, 65> last_lby;
};

extern c_anticheat anticheat;