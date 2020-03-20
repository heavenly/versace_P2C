#pragma once
#include "singleton.hpp"
#include "helpers/utils.hpp"
#include <array>

class far_esp : public Singleton<far_esp>
{
public:
	void Start();

	struct snd_player
	{
		void Override(const SndInfo_t sound)
		{
			origin = *sound.m_pOrigin;
			receive_time = Utils::get_epoch_time();
		}

		long long receive_time = 0;
		Vector origin = Vector(0, 0, 0);
		bool dormant = false;
	};
	
	std::array<snd_player, 65> sound_players;

	void clear();

private:

	void adjust_player_begin(C_BasePlayer* player);
	void setup_adjust_player(C_BasePlayer* player, SndInfo_t sound);
	bool valid_sound(SndInfo_t sound);


	CUtlVector<SndInfo_t> sound_buffer;
	CUtlVector<SndInfo_t> sound_list;
};
