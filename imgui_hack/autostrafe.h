#pragma once
#include "valve_sdk\csgostructs.hpp"

class autostrafer
{
public:
	void auto_jump(CUserCmd* cmd);
	void do_strafes(CUserCmd* cmd);
	void edge_jump(CUserCmd* cmd);
};