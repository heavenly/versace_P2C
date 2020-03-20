#pragma once
#include "valve_sdk/csgostructs.hpp"
#include "helpers/math.hpp"
#include <vector>

class c_walkbot
{
public:
	void walk(CUserCmd* cmd);
	void add_to_vector();
	void get_best_position();
	int current_position = 0;
private:
	bool local_status;
	std::vector<Vector> positions;
};
