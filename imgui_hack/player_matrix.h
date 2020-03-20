#pragma once
#include "valve_sdk/math/VMatrix.hpp"

namespace etc_player_info
{
	extern int backtrack_tick[65];
};

class matrixes
{
public:
	matrix3x4_t matrix[128];
};

extern matrixes player_matrixes[65];