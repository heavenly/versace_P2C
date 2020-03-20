#pragma once
#include "valve_sdk/csgostructs.hpp"

namespace Glow
{
	void render_glow();
	void clear_glow();
	void add_to_glow_mgr(ClientFrameStage_t stage);
}