#pragma once
#include "valve_sdk/csgostructs.hpp"

class c_misc_features
{
public:
	void do_quickswitch(CUserCmd* cmd);
	void log_quickswitch(CUserCmd* cmd);
	void infinite_crouch(CUserCmd* cmd);
	void restrict_speed(CUserCmd * cmd);
	void change_clantag();
	void auto_derank();
	void set_fakelag_limit();
	void crash_server();
	void copy_movement_bot(CUserCmd * cmd);
};