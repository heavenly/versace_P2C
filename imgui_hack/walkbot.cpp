#include "walkbot.h"
#include <chrono>
#include "visuals_pt.h"
#include "options.hpp"

void c_walkbot::walk(CUserCmd * cmd)
{
	if (!g_vars.misc_walkbot)
		return;

	if (!g_vars.misc_walkbot_walk)
		return;

	if (positions.size() < 2)
		return;

	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!cmd || !g_LocalPlayer)
		return;

	if (!g_LocalPlayer->is_alive())
		return;

	if (current_position > positions.size() - 1)
		current_position = 0;

	const Vector target = positions.at(current_position);
	QAngle aim_pos = Math::CalcAngle(g_LocalPlayer->get_eye_pos(), target);

	Math::Clamp(aim_pos);
	cmd->viewangles = aim_pos;
	cmd->forwardmove = 450;

	const auto distance_to_meters = [=](const Vector s, const Vector d) -> float
	{
		const auto dist = sqrt(s.DistToSqr(d));
		return dist * 0.01905f;
	};

	const float dist_3d = distance_to_meters(g_LocalPlayer->m_vecOrigin(), target);
	if (dist_3d <= .5f)
		current_position++;
}

void c_walkbot::add_to_vector()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!g_LocalPlayer || !g_LocalPlayer->is_alive())
		return;

	if (!g_vars.misc_walkbot || g_vars.misc_walkbot_add_key <= 0 || g_vars.misc_walkbot_erase_key <= 0)
		return;

	static long long last_erase_press_time;
	static long long last_log_press_time;

	auto epoch_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	const auto distance_to_meters = [=](const Vector s, const Vector d) -> float
	{
		const auto dist = sqrt(s.DistToSqr(d));
		return dist * 0.01905f;
	};

	int screen_width, screen_height;
	g_EngineClient->GetScreenSize(screen_width, screen_height);

	screen_width /= 2;
	screen_height /= 2;

	bool pos_erased = false;
	bool pos_found = false;

	for (int i = 0; i < positions.size(); i++)
	{
		Vector position = positions.at(i);
		Color active_clr = g_vars.color_walkbot_etc;

		float dist_3d = distance_to_meters(g_LocalPlayer->m_vecOrigin(), position);
		if (dist_3d <= 1)
			active_clr = g_vars.color_walkbot_next;

		Vector pos_2d;
		if (Math::WorldToScreen(position, pos_2d))
		{
			float dist_2d = pos_2d.DistTo(Vector(screen_width, screen_height, 0));

			if (dist_2d <= 15 && !pos_found)
			{
				pos_found = true;

				if (GetAsyncKeyState(g_vars.misc_walkbot_erase_key) && epoch_time - last_erase_press_time > 500 && !pos_erased)
				{
					last_erase_press_time = epoch_time;
					positions.erase(positions.begin() + i);
					pos_erased = true;
					continue;
				}
			}
		}

		Ray_t ray;
		trace_t tr;
		CTraceFilter filter;

		ray.Init(g_LocalPlayer->get_eye_pos(), position);
		filter.pSkip = g_LocalPlayer;
		g_EngineTrace->TraceRay(ray, 0x46004003, &filter, &tr);

		visuals_pt::draw_3d_box(7.f, g_LocalPlayer->m_angEyeAngles(), position, active_clr);
	}

	if (GetAsyncKeyState(g_vars.misc_walkbot_add_key) && epoch_time - last_log_press_time > 500 && !pos_erased)
	{
		last_log_press_time = epoch_time;
		Vector trace_start, trace_end;
		QAngle view_angles;

		g_EngineClient->GetViewAngles(&view_angles);
		Math::AngleVectors(view_angles, trace_end);

		trace_start = g_LocalPlayer->get_eye_pos();
		trace_end = trace_start + (trace_end * 8192);

		Ray_t ray;
		trace_t tr;
		CTraceFilter filter;

		ray.Init(trace_start, trace_end);
		filter.pSkip = g_LocalPlayer;
		g_EngineTrace->TraceRay(ray, 0x46004003, &filter, &tr);

		Vector pos = tr.endpos;
		positions.push_back(pos);
	}
}

void c_walkbot::get_best_position()
{

	if (!g_vars.misc_walkbot)
		return;

	if (!g_vars.misc_walkbot_walk)
		return;

	if (!g_vars.misc_walkbot_adaptive)
		return;

	if (positions.size() < 2)
		return;

	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!g_LocalPlayer)
		return;

	if (!g_LocalPlayer->is_alive())
	{
		local_status = false;
		return;
	}

	if (g_LocalPlayer->is_alive() && local_status)
		return;

	local_status = true;

	const auto local_origin = g_LocalPlayer->m_vecOrigin();
	auto best_dist = 9999999;

	for (int i = 0; i < positions.size(); i++)
	{
		auto pos = positions.at(i);
		const auto dist = sqrt(local_origin.DistToSqr(pos));

		if (dist < best_dist)
		{
			current_position = i;
			best_dist = dist;
		}
	}
}
