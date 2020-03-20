#include "animfix.h"

void c_animfix::do_animfix()
{
	//this shit is so fucking incorrect
	return;
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!g_LocalPlayer)
		return;

	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* entity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));

		if (!entity)
			continue;

		if (entity->is_alive() || entity->is_dormant())
			continue;

		const auto simtime = entity->GetSimulationTime();
		auto origin = entity->m_vecOrigin();
		const auto current_struct = &anims[entity->EntIndex()];

		//fix up origin + feet doing the nae nae + velocity (abs+networked vel = animstate vel)
		if (simtime != current_struct->last_simtime)
		{
			auto animstate = entity->get_base_player_animstate();
			QAngle proper_local = QAngle(animstate->m_flEyePitch, animstate->m_flGoalFeetYaw, 0);
			QAngle proper_entity = QAngle(animstate->m_flEyePitch, animstate->m_flEyeYaw, 0);

			auto prev_anim_layers = entity->GetAnimOverlays();
			current_struct->last_simtime = simtime;

			entity->set_abs_origin(origin);
			entity->set_abs_angle(entity == g_LocalPlayer ? proper_local : proper_entity);
			entity->UpdateClientSideAnimation();
			std::memcpy(entity->GetAnimOverlays(), prev_anim_layers, (sizeof(AnimationLayer) * entity->GetNumAnimOverlays()));
		}
	}
}