#include "far_esp.h"
#include "valve_sdk/csgostructs.hpp"
#include "options.hpp"

void far_esp::Start()
{
	if (!g_vars.esp_farther)
		return;

	sound_list.RemoveAll();
	g_EngineSound->GetActiveSounds(sound_list);

	if (sound_list.Count() <= 0)
		return;

	for (int i = 1; i < sound_list.Count(); i++)
	{
		SndInfo_t& sound = sound_list.Element(i);

		if (sound.m_nSoundSource == 0 || sound.m_nSoundSource > 64)
		{
			if (g_vars.misc_hide_ambient_sounds)
			{
				g_EngineSound->StopSoundByGuid(sound.m_nGuid, false);
			}
			continue;
		}

		C_BasePlayer* player = (C_BasePlayer*)g_EntityList->GetClientEntity(sound.m_nSoundSource);

		if (!player || player == g_LocalPlayer || !sound.m_pOrigin->IsValid())
			continue;

		if (!player->is_alive())
			continue;

		if (!valid_sound(sound))
			continue;

		setup_adjust_player(player, sound);

		sound_players[sound.m_nSoundSource].Override(sound);
	}

	for (int iter = 1; iter < g_EngineClient->GetMaxClients(); iter++)
	{
		C_BasePlayer* player = (C_BasePlayer*)g_EntityList->GetClientEntity(iter);

		if (!player || !player->is_alive() || !player->is_dormant())
			continue;

		adjust_player_begin(player);
	}

	sound_buffer = sound_list;
}

void far_esp::setup_adjust_player(C_BasePlayer* player, const SndInfo_t sound)
{
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = player;
	const Vector src_3d = (*sound.m_pOrigin) + Vector(0, 0, 1);
	const Vector dst_3d = src_3d - Vector(0, 0, 100);
	ray.Init(src_3d, dst_3d);

	g_EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &tr);

	if (tr.allsolid)
		sound_players[sound.m_nSoundSource].receive_time = -1;

	sound_players[sound.m_nSoundSource].receive_time = Utils::get_epoch_time();
	sound_players[sound.m_nSoundSource].origin = ((tr.fraction < 0.97) ? tr.endpos : *sound.m_pOrigin);
}

void far_esp::clear()
{
	//need to figure out how to unload CUtlVector
}

void far_esp::adjust_player_begin(C_BasePlayer* player)
{
	if (!g_vars.esp_farther)
		return;

	constexpr int EXPIRE_DURATION = 1000; //1000 ms, 1s
	auto& sound_player = sound_players[player->EntIndex()];
	sound_player.dormant = (Utils::get_epoch_time() - sound_player.receive_time) > EXPIRE_DURATION;
}

bool far_esp::valid_sound(const SndInfo_t sound)
{
	// Use only valid sounds - server and not bad origin.
	if (!sound.m_bFromServer || !sound.m_pOrigin->IsValid())
		return false;

	if (sound_buffer.Count() <= 0)
		return true;

	for (int z = 1; z < sound_buffer.Count(); z++)
	{
		SndInfo_t& current_sound = sound_buffer.Element(z);

		if (current_sound.m_nGuid == sound.m_nGuid && current_sound.m_pOrigin == sound.m_pOrigin)
			return false;

	}	

	return true;
}