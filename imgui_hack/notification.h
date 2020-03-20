#pragma once
#include "valve_sdk/csgostructs.hpp"
#include <string>
#include "global.h"

struct Notification
{
	std::string Message = "";
	long long time_of_message = Utils::get_epoch_time();
	Color clr = Color(238, 232, 170, 255);
};

namespace push_notifications
{
	constexpr int EXPIRY_TIME = 3000;
	void Update();
	void Push(std::string Message, Color c = Color(238, 232, 170, 255));
};