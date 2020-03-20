#include "fixmove.h"

void c_fix_move::correct_movement(QAngle old_angles, CUserCmd* pCmd, float old_forwardmove, float old_sidemove)
{
	auto deg_2_rad = [](double degrees) -> double
	{
		return degrees * 4.0 * atan(1.0) / 180.0;
	};
	//side/forward move correction
	float delta_view = pCmd->viewangles.yaw - old_angles.yaw;

	float f1 = old_angles.yaw < 0.f ? 360.0f + old_angles.yaw : old_angles.yaw;

	float f2 = pCmd->viewangles.yaw < 0.f ? 360.0f + pCmd->viewangles.yaw : pCmd->viewangles.yaw;

	delta_view = f2 < f1 ? abs(f2 - f1) : 360.0f - abs(f1 - f2);
	delta_view = 360.0f - delta_view;

	pCmd->forwardmove = cos(deg_2_rad(delta_view)) * old_forwardmove + cos(deg_2_rad(delta_view + 90.f)) * old_sidemove;
	pCmd->sidemove = sin(deg_2_rad(delta_view)) * old_forwardmove + sin(deg_2_rad(delta_view + 90.f)) * old_sidemove;
}

void c_fix_move::Start(CUserCmd* pCmd)
{
	if (!pCmd)
		return;

	if (!g_LocalPlayer)
		return;

	if (!g_LocalPlayer->is_alive())
		return;

	old = pCmd->viewangles;
	old_forward_move = pCmd->forwardmove;
	old_side_move = pCmd->sidemove;
}

void c_fix_move::Stop(CUserCmd* pCmd)
{
	correct_movement(old, pCmd, old_forward_move, old_side_move);
}

QAngle c_fix_move::get_old_angle()
{
	return old;
}