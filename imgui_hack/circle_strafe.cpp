#include "circle_strafe.h"
#include "options.hpp"

float c_circle_strafe::get_trace_world_fractions(Vector start_pos, Vector end_pos) {
	Ray_t ray;
	trace_t tr;
	CTraceFilterWorldOnly filter;

	ray.Init(start_pos, end_pos);

	g_EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	return tr.fraction;
}

void c_circle_strafe::circle_strafer_main(CUserCmd* cmd)
{
	if (!g_vars.misc_circle_strafe)
		return;

	if (g_vars.misc_circle_strafe_on_key > 0 && !GetAsyncKeyState(g_vars.misc_circle_strafe_on_key))
		return;

	static float strafe_angle;

	Vector velocity = g_LocalPlayer->m_vecVelocity();
	velocity.z = 0;

	float Speed = velocity.Length();

	if (Speed < 45)
		Speed = 45;

	if (Speed > 750)
		Speed = 750;


	float final_path = get_trace_world_fractions(g_LocalPlayer->m_vecOrigin() + Vector(0, 0, 10), g_LocalPlayer->m_vecOrigin() + Vector(0, 0, 10) + velocity / 2.0f);
	float delta_angle = fmax((275.0f / Speed) * (2.0f / final_path) * (128.0f / (1.7f / g_GlobalVars->interval_per_tick)) * 4.f, 2.0f);
	strafe_angle += delta_angle;

	if (fabs(strafe_angle) >= 360.0f)
		strafe_angle = 0.0f;
	else {
		constexpr auto m_pi = 3.14159265358979323846f;
		cmd->forwardmove = cos((strafe_angle + 90) * (m_pi / 180.0f)) * 450.f;
		cmd->sidemove = sin((strafe_angle + 90) * (m_pi / 180.0f)) * 450.f;
	}
}