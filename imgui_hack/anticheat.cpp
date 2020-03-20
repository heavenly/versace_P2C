#include "anticheat.h"
#include "valve_sdk/csgostructs.hpp"
#include "options.hpp"
#include "resolver.h"

c_anticheat anticheat = c_anticheat();

void c_anticheat::calculate_choked_ticks()
{
	//call in CM
	for (size_t i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		auto* p_entity = dynamic_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));

		if (!p_entity)
			continue;

		if (!p_entity->is_alive() || p_entity->is_dormant())
			continue;

		if (p_entity->GetSimulationTime() == last_simtime[i])
			players[i].choked_ticks++;
		else
		{
			players[i].choked_ticks = 0;
			last_simtime[i] = p_entity->GetSimulationTime();
		}
	}
}

void c_anticheat::calculate_angle_deltas()
{
	//call in cm
	for (size_t i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		auto* p_entity = dynamic_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));

		if (!p_entity)
			continue;

		if (!p_entity->is_alive() || p_entity->is_dormant() || p_entity->m_vecVelocity().Length() > 1)
			continue;

		//retarded nn way
		const int retards_delta = p_entity->m_angEyeAngles().yaw - p_entity->m_flLowerBodyYawTarget();
		const int actual_delta = ((retards_delta + 180) % 360 + 360) % 360 - 180;
		players[i].angle_delta = abs(actual_delta);
	}
}

void c_anticheat::do_createmove_log()
{
	calculate_angle_deltas();
	calculate_choked_ticks();
}

void c_anticheat::fix_players()
{
	//call in fsn pdu start

	if (!g_vars.misc_resolver)
		return;

	for (size_t i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		auto* entity = dynamic_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
		if (!entity)
			continue;

		if (!entity->is_alive() || entity->is_dormant())
			continue;

		resolver.do_resolver();
	}
}

void c_anticheat::do_lby_pred()
{
	for (size_t i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		auto* entity = dynamic_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
		if (!entity)
			continue;

		if (!entity->is_alive() || entity->is_dormant())
			continue;

		const auto lby = entity->m_flLowerBodyYawTarget();

		if (last_lby[i] != lby)
		{
			//last animtime
			best_time[i] = entity->m_flOldSimulationTime() + g_GlobalVars->interval_per_tick;
			entity->m_angEyeAngles().yaw = lby;
		}

		if (entity->GetSimulationTime() - best_time[i] > 1.1 && entity->m_vecVelocity().Length2D() < 1)
		{
			best_time[i] = entity->GetSimulationTime();
			if (g_vars.misc_predict_enemy_lby)
			{
				entity->m_angEyeAngles().yaw = lby;
			}
		}
	}
}
