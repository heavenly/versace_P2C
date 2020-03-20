#pragma once
#include "valve_sdk/csgostructs.hpp"
#include <array>
struct animation_info
{
	float last_simtime;
};

class c_animfix
{
public:
	void do_animfix();
	std::array<animation_info, 65> anims;
};