#include "sonar.h"
#include "options.hpp"
#include "valve_sdk/Math/Vector.hpp"
#include "valve_sdk/csgostructs.hpp"

void c_sonar::do_sonar()
{
	if (!g_vars.misc_sonar)
		return;

	const auto distance = [](Vector a, Vector b) -> float
	{
		return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
	};

	float dist = FLT_MAX;
	static float last_beep = 0;
	
	for (size_t i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* ent = C_BasePlayer::GetPlayerByIndex(i);
		if (!ent)
			continue;

		if (!ent->is_alive() || ent->is_dormant())
			continue;

		if (ent->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
			continue;

		const Vector local_pos = g_LocalPlayer->get_eye_pos();
		const Vector enemy_pos = ent->get_eye_pos();

		auto dist_from_local = distance(local_pos, enemy_pos);

		dist = dist_from_local < dist ? dist_from_local : dist; //lol moment
	}

	if (last_beep + (dist / 850) <= g_GlobalVars->curtime)
	{
		g_EngineClient->ExecuteClientCmd("play buttons\\bell1.wav");
		last_beep = g_GlobalVars->curtime;
	}
}
