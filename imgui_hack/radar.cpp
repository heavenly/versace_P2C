#include "radar.h"
#include "valve_sdk/csgostructs.hpp"
#include "options.hpp"
void c_radar::do_radar()
{
	if (!g_vars.misc_radar)
		return;

	for (size_t i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

		if (!entity)
			continue;

		if (entity->is_dormant() || !entity->is_alive())
			continue;

		entity->m_bSpotted() = true;
	}
}
