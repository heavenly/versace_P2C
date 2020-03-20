#pragma once
#include "valve_sdk/misc/Color.hpp"

namespace etc_functions
{
	extern bool color_edit_4(const char* label, Color* v, bool show_alpha = true);
	extern bool ColorPicker(const char *label, Color *col);
};