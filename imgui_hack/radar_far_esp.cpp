#include "radar_far_esp.h"

std::array<player_radar_logs, 65> c_radar_far_esp::radar_logs = {};

void c_radar_far_esp::cache_players()
{
	static auto dw_radar_base = *(DWORD*)(Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), "A1 ? ? ? ? 8B 0C B0 8B 01 FF 50 ? 46 3B 35 ? ? ? ? 7C EA 8B 0D").value() + 1);
	static DWORD radar_base = *(DWORD*)(*(DWORD*) (dw_radar_base) + 116);
	
	if (!radar_base)
		return;

	for (int i = 0; i < g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* entity = C_BasePlayer::GetPlayerByIndex(i);

		if (!entity || !entity->is_alive() || entity == g_LocalPlayer || entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
			continue;

		const auto radar_player = *reinterpret_cast<radar_player_t*>(radar_base + (0x174 * (i + 1)) - 0x3C);
		auto& current_radar_log = radar_logs[i];
		
		if (radar_player.visible != current_radar_log.last_visible_status || radar_player.origin != current_radar_log.origin)
		{
			current_radar_log.last_visible_status = radar_player.visible;

			if (radar_player.visible == 3 || radar_player.visible == 1) //spotted by me or teammate
			{
				current_radar_log.last_spotted_time = Utils::get_epoch_time();
				current_radar_log.origin = radar_player.origin;
			}
		}

		current_radar_log.is_valid_log = Utils::get_epoch_time() - current_radar_log.last_spotted_time < EXPIRY_TIME;
	}
}
