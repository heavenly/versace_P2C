#pragma once
#include "valve_sdk/csgostructs.hpp"

class c_antiaim
{
public:
	void anti_trigger(CUserCmd* cmd);
	void anti_trigger_choke(bool& bSendPacket);
	void do_antiaim(CUserCmd* cmd, bool& bSendPacket);
	void do_fakelag(CUserCmd* cmd, bool& bSendPacket);
private:
	bool _should_choke;
	float get_max_desync_delta();
};