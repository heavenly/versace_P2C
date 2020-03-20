#pragma once
#include "valve_sdk/csgostructs.hpp"
#include "player_matrix.h"
#include <deque>
#include <array>

struct backtrack_record
{
	Vector head;
	Vector origin;
	matrixes ent_matrix;
	int tick;
};

struct backtrack_aim_data
{
	C_BasePlayer* entity;
	int best_tick;
	QAngle aim_pos;
	bool has_better_target;
};

class c_backtracking
{
	C_BasePlayer* best_entity;
	matrixes best_matrix;

public:
	backtrack_aim_data get_best_backtrack_aim_tick(CUserCmd* cmd, float best_fov);
	std::array<std::deque<backtrack_record>, 65> entity_records;
	void register_tick(CUserCmd* cmd);
	void clean_records(CUserCmd* cmd);
	void Begin(CUserCmd* cmd);
	void End();
	void get_best_hvh_backtrack_tick(C_BasePlayer* entity, CUserCmd* cmd);
	bool is_over_player_backtrack_record(CUserCmd* cmd);
	void draw_backtrack_skeleton(C_BasePlayer* entity);

private:
	std::optional<Vector> get_intersection_point(const Vector& start, const Vector& end, const Vector& mins, const Vector& maxs, float radius);

};
extern c_backtracking backtrack;