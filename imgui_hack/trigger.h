#pragma once
#include "valve_sdk/csgostructs.hpp"

class c_trigger
{
public:
	void do_trigger(CUserCmd* cmd);
	bool is_over_player(CUserCmd* cmd);

private:
	long long last_epoch;
};

extern c_trigger trigger;