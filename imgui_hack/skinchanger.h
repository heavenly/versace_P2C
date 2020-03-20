#pragma once
#include "valve_sdk/csgostructs.hpp"
#include <array>
struct skin_info
{
	int skin_id;
	float wear;
	int seed;
};

class c_skinchanger
{
public:
	void do_skin_change();
	std::array<skin_info, 512> info;
};

extern c_skinchanger skinchanger;