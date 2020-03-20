#include "legit_aa.h"
#include "helpers/math.hpp"
#include <algorithm>
#include "options.hpp"

float c_antiaim::get_max_desync_delta() {
	const auto animstate = uintptr_t(g_LocalPlayer->get_base_player_animstate());

	const float duck_amount = *(float*)(animstate + 0xA4);
	const float speed_fraction = std::max(0.f, std::min(*reinterpret_cast<float*>(animstate + 0xF8), 1.f));

	const float speed_factor = std::max(0.f, std::min(1.f, *reinterpret_cast<float*> (animstate + 0xFC)));

	const float unk1 = ((*reinterpret_cast<float*> (animstate + 0x11C) * -0.30000001) - 0.19999999) * speed_fraction;
	float unk2 = unk1 + 1.f;

	if (duck_amount > 0)
		unk2 += ((duck_amount * speed_factor) * (0.5f - unk2));

	const float unk3 = *(float*)(animstate + 0x334) * unk2;
	return unk3;
}

Vector vect2d(QAngle p1, QAngle p2) {
	Vector temp;
	temp.x = (p2.pitch - p1.pitch);
	temp.y = -1 * (p2.yaw - p1.yaw);

	return temp;
}

bool point_in_rect(QAngle A, QAngle B, QAngle C, QAngle D, QAngle m) {
	Vector AB = vect2d(A, B);  float C1 = -1 * (AB.y * A.pitch + AB.x * A.yaw); float  D1 = (AB.y * m.pitch + AB.x * m.yaw) + C1;
	Vector AD = vect2d(A, D);  float C2 = -1 * (AD.y * A.pitch + AD.x * A.yaw); float D2 = (AD.y * m.pitch + AD.x * m.yaw) + C2;
	Vector BC = vect2d(B, C);  float C3 = -1 * (BC.y * B.pitch + BC.x * B.yaw); float D3 = (BC.y * m.pitch + BC.x * m.yaw) + C3;
	Vector CD = vect2d(C, D);  float C4 = -1 * (CD.y * C.pitch + CD.x * C.yaw); float D4 = (CD.y * m.pitch + CD.x * m.yaw) + C4;
	return     0 >= D1 && 0 >= D4 && 0 <= D2 && 0 >= D3;
}

bool check_if_between(QAngle* ViewanglePoints, QAngle Viewangles)
{
	if (point_in_rect(ViewanglePoints[0], ViewanglePoints[1], ViewanglePoints[2], ViewanglePoints[3], Viewangles) //first 4 normal
		|| point_in_rect(ViewanglePoints[3], ViewanglePoints[2], ViewanglePoints[1], ViewanglePoints[0], Viewangles) //first 4 reverse
		|| point_in_rect(ViewanglePoints[4], ViewanglePoints[5], ViewanglePoints[6], ViewanglePoints[7], Viewangles) //second 4 normal
		|| point_in_rect(ViewanglePoints[7], ViewanglePoints[6], ViewanglePoints[5], ViewanglePoints[4], Viewangles))//second 4 reverse
	{
		return true;
	}
	return false;
}

Vector extrapolate(C_BasePlayer* player, int ticks)
{
	return player->m_vecOrigin() + (player->m_vecVelocity() * (g_GlobalVars->interval_per_tick * ticks));
}


void c_antiaim::anti_trigger(CUserCmd* cmd)
{
	if (!g_LocalPlayer || !g_LocalPlayer->is_alive() || g_LocalPlayer->m_vecVelocity().Length2D() < 100 || _should_choke) // skip function if we are dead or not really moving
		return;

	for (int i = 0; i < g_EngineClient->GetMaxClients(); i++) // loop though maxent
	{
		C_BasePlayer* entity = C_BasePlayer::GetPlayerByIndex(i);

		if (!entity || !entity->is_alive() || entity->is_dormant())
			continue;

		if (entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
			continue;

		if (entity->m_vecVelocity().Length2D() > 50)
			continue;

		auto entity_weapon = entity->m_hActiveWeapon();

		if (entity_weapon->is_grenade() || entity_weapon->is_knife() || entity_weapon->is_taser())
			continue;

		QAngle ent_view_angs = entity->m_angEyeAngles(); // get their viewangles
		Vector forward_vec; //placeholder

		Math::AngleVectors(ent_view_angs, forward_vec);
		forward_vec *= 8012.f;

		Vector
			start = entity->get_eye_pos(),
			end = start + forward_vec;

		auto
			org = extrapolate(g_LocalPlayer, 2), // this prolly needs adjusting, extrapolt urself by x ticks
			min = g_LocalPlayer->GetCollideable()->OBBMins(),
			max = g_LocalPlayer->GetCollideable()->OBBMaxs();
		Vector boxaroundme[8] = { //the comments below are not accurate
			Vector(org.x + min.x, org.y + min.y, org.z + min.z), // left buttom
			Vector(org.x + min.x, org.y + min.y, org.z + max.z), // left top
			Vector(org.x + max.x, org.y + max.y, org.z + max.z), // right top
			Vector(org.x + max.x, org.y + max.y, org.z + min.z),  // right buttom

			Vector(org.x + min.x, org.y + max.y, org.z + min.z), // left buttom
			Vector(org.x + min.x, org.y + max.y, org.z + max.z), // left top
			Vector(org.x + max.x, org.y + min.y, org.z + max.z), // right top
			Vector(org.x + max.x, org.y + min.y, org.z + min.z)  // right buttom
		};

		QAngle Angles[8];
		for (int j = 0; j < 8; j++) { //get angle to each rectangle corner
			Angles[j] = Math::CalcAngle(start, boxaroundme[j]);
		}

		if (!check_if_between(Angles, ent_view_angs))
			continue; //skip entity if viewangles outside of rect

		trace_t tr;
		Ray_t ray;
		CTraceFilter filter;
		filter.pSkip = entity;

		ray.Init(start, end);
		g_EngineTrace->TraceRay(ray, 0x46004003, &filter, &tr);//normal raycasting, u might wanna change the mask so the ray doesnt collide with hitboxes

		if ((tr.endpos - start).Length() + 10 >= (g_LocalPlayer->get_eye_pos() - start).Length()) { //pretty ghetto intersect check
			_should_choke = true; // globalvar for should choke
			break;
		}
	}
}

void c_antiaim::anti_trigger_choke(bool& bSendPacket)
{
	if (!_should_choke)
		return;

	static int _pchoked = 0;
	if (!_pchoked && !bSendPacket)
		_pchoked++;

	if (_pchoked <= 13) {
		bSendPacket = false;
		_pchoked++;
	}
	else {
		static int repeats = 0;
		if (repeats >= 1)
			_should_choke = false;
		repeats++;
		bSendPacket = true;
		_pchoked = 0;
	}
}

void c_antiaim::do_antiaim(CUserCmd* cmd, bool& bSendPacket)
{
	if (!g_LocalPlayer || !cmd)
		return;

	if (cmd->buttons & IN_ATTACK || cmd->buttons & IN_USE)
		return;

	if (!g_vars.misc_legit_aa)
		return;

	QAngle local_view;
	g_EngineClient->GetViewAngles(&local_view);
	/*if(lby_should_update)
	{
		cmd->viewangles.yaw = (cmd->viewangles.yaw + 180) - get_max_desync_delta(); //fake
	}
	else*/ if (bSendPacket) //fake
		cmd->viewangles.yaw = local_view.yaw;
	else //real
		cmd->viewangles.yaw = (local_view.yaw + 180) - (g_vars.misc_legit_aa_ang < get_max_desync_delta() ? g_vars.misc_legit_aa_ang : get_max_desync_delta());

	if (g_vars.misc_legit_aa_pitch)
		cmd->viewangles.pitch = g_vars.misc_legit_aa_pitch_val;
}

void c_antiaim::do_fakelag(CUserCmd* cmd, bool& bSendPacket)
{
	if (cmd->buttons & IN_ATTACK || cmd->buttons & IN_USE)
		return;

	if (!g_vars.misc_legit_aa_fakelag)
		return;

	static int choked_ticks = 0;

	bSendPacket = choked_ticks > g_vars.misc_legit_aa_fakelag_amount;
	choked_ticks = bSendPacket ? 0 : ++choked_ticks;
}
