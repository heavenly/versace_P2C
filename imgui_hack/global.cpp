#include "global.h"

namespace global_vars
{
	duk_context* ctx = duk_create_heap_default();
	CUserCmd* cmd;
	ClientFrameStage_t stage;
};