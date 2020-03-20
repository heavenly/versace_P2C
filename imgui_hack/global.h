#pragma once
#include "javascript_engine/duktape/duktape.h"
#include "valve_sdk/csgostructs.hpp"
namespace global_vars
{
	extern duk_context* ctx;
	extern CUserCmd* cmd;
	extern ClientFrameStage_t stage;
};