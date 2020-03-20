#include "rcs.h"
#include "options.hpp"
#include "helpers/math.hpp"
QAngle old_punch;
void c_recoil::do_recoil(CUserCmd * cmd)
{
	if (!g_vars.misc_rcs)
		return;

	if (g_vars.aimbot_enabled)
		return;

	if (!(cmd->buttons & IN_ATTACK))
		return;
	
	const QAngle cur_aim_punch = g_LocalPlayer->m_aimPunchAngle();
	if (g_LocalPlayer->m_iShotsFired() > 1)
	{
		const QAngle new_punch = QAngle(cur_aim_punch.pitch - old_punch.pitch, cur_aim_punch.yaw - old_punch.yaw, 0);
		cmd->viewangles -= new_punch * g_vars.misc_rcs_amount;
		Math::Clamp(cmd->viewangles);
		g_EngineClient->SetViewAngles(&cmd->viewangles);
	}
	old_punch = cur_aim_punch;
}
