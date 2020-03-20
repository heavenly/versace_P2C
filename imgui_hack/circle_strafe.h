#pragma once
#include "valve_sdk/csgostructs.hpp"

class c_circle_strafe
{
private:
	float get_trace_world_fractions(Vector start_pos, Vector end_pos);
public:
	void circle_strafer_main(CUserCmd* cmd);
};