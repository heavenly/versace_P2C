#pragma once
#include "json.h"

namespace Configs
{
	void save_cfg(std::string file_name);
	void load_cfg(std::string file_name);
	void load_from_web(std::string tag);
	void save_to_web(std::string tag);

	std::vector<std::string> get_top_10();
	std::vector<std::string> get_local_configs();
}