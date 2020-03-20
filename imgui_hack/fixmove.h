#pragma once
#include "valve_sdk\csgostructs.hpp"
class c_fix_move
{
public:
	void Start(CUserCmd*);
	void Stop(CUserCmd*);
	QAngle get_old_angle();
private:
	QAngle old;
	float old_forward_move;
	float old_side_move;
	void correct_movement(QAngle old_angles, CUserCmd* pCmd, float old_forwardmove, float old_sidemove);
};