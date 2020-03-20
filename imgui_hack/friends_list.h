#pragma once
#include <vector>
#include "SteamClientPublic.h"
namespace modified_friends_list
{
	extern std::vector<CSteamID> friends;
	extern void save_friends_to_file();
	extern void load_friends_from_file();
};