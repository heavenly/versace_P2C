#pragma once
#include "valve_sdk/csgostructs.hpp"
#include <array>
class c_beams
{
public:
	void draw_rings(C_BasePlayer* player);
private:
	std::array<float, 65> last_draw_time;
	std::array<Vector, 65> last_origin;
};