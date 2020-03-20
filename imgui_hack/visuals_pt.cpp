#include "visuals_pt.h"
#include "valve_sdk/csgostructs.hpp"
#include "options.hpp"
#include "helpers/math.hpp"
#include "helpers/utils.hpp"
#include "local_info.h"
#include <chrono>
#include "far_esp.h"
#include "radar_far_esp.h"
#include "autowall.h"

vgui::HFont esp_font;
vgui::HFont injection_font;
// ESP Context
// This is used so that we dont have to calculate player color and position
// on each individual function over and over
struct
{
	C_BasePlayer* pl;
	bool          is_enemy;
	bool			is_team;
	bool          is_visible;
	Color         clr;
	Vector        head_pos;
	Vector        feet_pos;
	RECT          bbox;
	int right_text_size = 0;
	int bottom_text_size = 0;
	player_info_t* pinfo;
	bool is_vs_user = false;
} esp_ctx;


void visuals_pt::draw_string(Color col, const char* str, int x, int y)
{
	wchar_t formatted[512] = { '\0' };
	wsprintfW(formatted, L"%S", str);
	g_VGuiSurface->DrawSetTextFont(esp_font);
	g_VGuiSurface->DrawSetTextPos(x, y);
	g_VGuiSurface->DrawSetTextColor(col);
	g_VGuiSurface->DrawPrintText(formatted, wcslen(formatted));
}

void visuals_pt::Player::draw_hitboxes(C_BasePlayer* entity, Color c)
{
	if (!entity)
		return;

	const auto index = entity->EntIndex();
	const auto matrix = player_matrixes[index].matrix;

	if (!matrix)
		return;

	if (!entity->GetModel())
		return;

	auto studio_mdl = g_MdlInfo->GetStudiomodel(entity->GetModel());

	if (!studio_mdl)
		return;

	auto hitbox_set = studio_mdl->GetHitboxSet(0);
	if (!hitbox_set.has_value())
		return;
	
	for (size_t i = 0; i < hitbox_set.value()->numhitboxes; i++)
	{
		auto hitbox = hitbox_set.value()->GetHitbox(i);
		if (!hitbox.has_value())
			continue;

		auto hitbox_value = hitbox.value();
		
		if (!hitbox_value || hitbox_value->m_flRadius <= 0)
			continue;

		
		Vector min, max;
		Math::VectorTransform(hitbox_value->bbmin, matrix[hitbox_value->bone], min);
		Math::VectorTransform(hitbox_value->bbmax, matrix[hitbox_value->bone], max);

		g_DebugOverlay->AddCapsuleOverlay(min, max, hitbox_value->m_flRadius, c.r(), c.g(), c.b(), c.a(), g_vars.esp_hurt_hitboxes_time);
	}
}

void visuals_pt::Player::draw_origin_box()
{
	if (!esp_ctx.pl->is_dormant() && g_vars.esp_origin_box_style == 1)
		return;
	
	if (Player::is_completely_dormant())
		return;

	auto index = esp_ctx.pl->EntIndex();

	auto player_info = far_esp::Get().sound_players[index];
	auto radar_player_info = c_radar_far_esp::radar_logs[index];

	auto draw_3d_cube = [=](float scalar, QAngle angles, Vector middle_origin, Color outline) -> void
	{
		Vector forward, right, up;
		Math::AngleVectors(angles, forward, right, up);

		Vector points[8];
		points[0] = middle_origin - (right * scalar) + (up * scalar) - (forward * scalar); // BLT
		points[1] = middle_origin + (right * scalar) + (up * scalar) - (forward * scalar); // BRT
		points[2] = middle_origin - (right * scalar) - (up * scalar) - (forward * scalar); // BLB
		points[3] = middle_origin + (right * scalar) - (up * scalar) - (forward * scalar); // BRB

		points[4] = middle_origin - (right * scalar) + (up * scalar) + (forward * scalar); // FLT
		points[5] = middle_origin + (right * scalar) + (up * scalar) + (forward * scalar); // FRT
		points[6] = middle_origin - (right * scalar) - (up * scalar) + (forward * scalar); // FLB
		points[7] = middle_origin + (right * scalar) - (up * scalar) + (forward * scalar); // FRB

		Vector points_screen[8];
		for (int i = 0; i < 8; i++)
			if (!Math::WorldToScreen(points[i], points_screen[i]))
				return;

		g_VGuiSurface->DrawSetColor(outline);

		// Back frame
		g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[1].x, points_screen[1].y);
		g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[2].x, points_screen[2].y);
		g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[1].x, points_screen[1].y);
		g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[2].x, points_screen[2].y);

		// Frame connector
		g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[4].x, points_screen[4].y);
		g_VGuiSurface->DrawLine(points_screen[1].x, points_screen[1].y, points_screen[5].x, points_screen[5].y);
		g_VGuiSurface->DrawLine(points_screen[2].x, points_screen[2].y, points_screen[6].x, points_screen[6].y);
		g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[7].x, points_screen[7].y);

		// Front frame
		g_VGuiSurface->DrawLine(points_screen[4].x, points_screen[4].y, points_screen[5].x, points_screen[5].y);
		g_VGuiSurface->DrawLine(points_screen[4].x, points_screen[4].y, points_screen[6].x, points_screen[6].y);
		g_VGuiSurface->DrawLine(points_screen[7].x, points_screen[7].y, points_screen[5].x, points_screen[5].y);
		g_VGuiSurface->DrawLine(points_screen[7].x, points_screen[7].y, points_screen[6].x, points_screen[6].y);
	};

	draw_3d_cube(g_vars.esp_origin_box_size, esp_ctx.pl->m_angEyeAngles(), Player::get_best_origin(), g_vars.color_origin_box);
}

void visuals_pt::Player::draw_money_esp()
{
	const int x = esp_ctx.bbox.right + 5;
	const int y = esp_ctx.bbox.top + (esp_ctx.right_text_size * 10);
	esp_ctx.right_text_size++;

	std::string money = "$" + std::to_string(esp_ctx.pl->m_iAccount());

	draw_string(Color(51, 153, 51, esp_ctx.clr.a()), money.c_str(), x, y);
}

void visuals_pt::Player::draw_viewangles()
{
	Vector forward;
	Vector src;
	Vector dst;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	Math::AngleVectors(esp_ctx.pl->m_angEyeAngles(), forward);
	filter.pSkip = esp_ctx.pl;
	const Vector src_3d = esp_ctx.pl->get_hitbox_pos(Hitboxes::HITBOX_HEAD);
	const Vector dst_3d = src_3d + (forward * g_vars.esp_viewangles_length);

	ray.Init(src_3d, dst_3d);

	g_EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	if (!Math::WorldToScreen(src_3d, src) || !Math::WorldToScreen(tr.endpos, dst))
		return;

	const auto local = esp_ctx.pl == g_LocalPlayer;
	const auto team = esp_ctx.pl->m_iTeamNum() == g_LocalPlayer->m_iTeamNum() && !local;
	const auto enemy = !team;
	
	int style = 0;
	if (team)
		style = 0;
	else if (enemy)
		style = 1;
	else if (local)
		style = 2;

	g_VGuiSurface->DrawSetColor(g_vars.color_viewangles[style]);
	g_VGuiSurface->DrawLine(src.x, src.y, dst.x, dst.y);
}

void visuals_pt::Player::draw_distance()
{
	auto dist = [=](Vector a, Vector b) -> int
	{
		return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
	};

	auto hu_to_feet = [=](int hu) -> float
	{
		return hu * .0625;
	};

	auto hu_to_meters = [=](int hu) -> float
	{
		return (hu * 1.904) / 1000;
	};

	const int x = esp_ctx.bbox.right + 5;
	const int y = esp_ctx.bbox.top + (esp_ctx.right_text_size * 10);
	esp_ctx.right_text_size++;

	auto d = dist(g_LocalPlayer->m_vecOrigin(), esp_ctx.pl->m_vecOrigin());

	std::string distance = g_vars.esp_distance_style == 0 ? std::to_string(hu_to_feet(d)) : std::to_string(hu_to_meters(d));
	distance += g_vars.esp_distance_style == 0 ? " feet" : " meters";

	draw_string(esp_ctx.clr, distance.c_str(), x, y);
}

void visuals_pt::Player::render_defuser()
{
	const int x = esp_ctx.bbox.right + 5;
	const int y = esp_ctx.bbox.top + (esp_ctx.right_text_size * 10);
	esp_ctx.right_text_size++;

	if (esp_ctx.pl->m_bHasDefuser() && g_vars.esp_player_defuser)
		draw_string(esp_ctx.clr, "kit", x, y);

	if (esp_ctx.pl->HasC4() && g_vars.esp_player_c4)
		draw_string(esp_ctx.clr, "C4", x, y);
}

void visuals_pt::Player::render_player_bomb_interactions()
{
	bool can_continue = false;
	float weight = 0;
	bool defusing = false;

	for (int i = 0; i < esp_ctx.pl->GetNumAnimOverlays(); i++)
	{
		auto seq_anim = esp_ctx.pl->GetSequenceActivity(esp_ctx.pl->GetAnimOverlay(i).value()->m_nSequence);
		float seq_weight = esp_ctx.pl->GetAnimOverlay(i).value()->m_flWeight;

		if (seq_anim == 958 || seq_anim == 959 || seq_anim == 978)
		{
			defusing = seq_anim == 958 || seq_anim == 959;
			weight = seq_weight;
			can_continue = true;
		}
	}

	if (!can_continue)
		return;

	if (weight == 0)
		return;


	const int left = esp_ctx.bbox.left;
	const int right = esp_ctx.bbox.right;
	const float percent = weight / 1.f;
	const auto black = Color(0, 0, 0, 130);
	const int y = esp_ctx.feet_pos.y + (esp_ctx.bottom_text_size * 11);

	//outline
	g_VGuiSurface->DrawSetColor(black);
	g_VGuiSurface->DrawFilledRect(left, y, right, y + 4);

	const auto width = (esp_ctx.bbox.right - esp_ctx.bbox.left) - 1;
	g_VGuiSurface->DrawSetColor(defusing ? g_vars.color_defusing_bar : g_vars.color_planting_bar);
	g_VGuiSurface->DrawFilledRect(left + 1, y + 1, left + width * (percent), y + 3);

	esp_ctx.bottom_text_size++;
}

void visuals_pt::Misc::render_offscreen_esp()
{
	if (!g_vars.esp_offscreen)
		return;

	auto draw_polygon = [](int count, Vertex_t* Vertexs, Color color)
	{
		static int Texture = g_VGuiSurface->CreateNewTextureID(true);
		unsigned char buffer[4] = { color.r(), color.g(), color.b(), color.a() };

		g_VGuiSurface->DrawSetTextureRGBA(Texture, buffer, 1, 1);
		g_VGuiSurface->DrawSetColor(color);
		g_VGuiSurface->DrawSetTexture(Texture);

		g_VGuiSurface->DrawTexturedPolygon(count, Vertexs);
	};

	auto draw_triangle = [=](int x, int y, float w, float h, float angle, Color color)
	{
		std::vector<Vertex_t> vertices;
		float rad = DEG2RAD(angle), rad_flip = DEG2RAD(angle + 90);

		vertices.emplace_back(Vertex_t(Vector2D((w / 2) * cos(rad_flip) + x, (w / 2) * sin(rad_flip) + y)));
		vertices.emplace_back(Vertex_t(Vector2D((-w / 2) * cos(rad_flip) + x, (-w / 2) * sin(rad_flip) + y)));
		vertices.emplace_back(Vertex_t(Vector2D(h * cos(rad) + x, h * sin(rad) + y)));

		draw_polygon(3, vertices.data(), color);
	};

	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		auto player = C_BasePlayer::GetPlayerByIndex(i);

		if (!player || !player->IsPlayer() || !player->is_alive())
			continue;

		if (player->is_dormant())
			continue;

		if (player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum() && !g_vars.esp_targets[0])
			continue;

		if (player->m_iTeamNum() != g_LocalPlayer->m_iTeamNum() && !g_vars.esp_targets[1])
			return;

		if (player->EntIndex() == g_LocalPlayer->EntIndex())
			continue;

		auto eye_pos = player->get_eye_pos();
		Vector out;

		//player is on screen / visible
		if (Math::WorldToScreen(eye_pos, out))
			continue;

		Vector vec;
		QAngle eye_angs;
		int screen_w, screen_h;

		auto origin = player->m_vecOrigin();

		g_EngineClient->GetScreenSize(screen_w, screen_h);
		g_EngineClient->GetViewAngles(&eye_angs);

		auto local_origin = g_LocalPlayer->m_vecOrigin(); 
		local_origin.z = 0;
		origin.z = 0;
		

		QAngle new_angle = Math::CalcAngle(local_origin, origin);
		Math::AngleVectors(QAngle(0, 270, 0) - new_angle + QAngle(0, eye_angs.yaw, 0), vec);

		auto circle_vec = Vector(screen_w / 2, screen_h / 2, 0) + (vec * ((screen_h / 2) - (screen_h / 10)));
		draw_triangle(circle_vec.x, circle_vec.y, g_vars.esp_offscreen_width, g_vars.esp_offscreen_height, 270 - new_angle.yaw + eye_angs.yaw, g_vars.color_offscreen_esp);
	}
}

RECT get_bbox(C_BaseEntity* ent)
{

	RECT rect{};

	if (!ent)
		return rect;

	auto collideable = ent->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	const matrix3x4_t& trans = ent->m_rgflCoordinateFrame();

	Vector points[] = {
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++) {
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++) {
		if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
			return rect;
	}

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++) {
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top < screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}
	return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}

bool visuals_pt::create_fonts()
{
	esp_font = g_VGuiSurface->CreateFont_();
	injection_font = g_VGuiSurface->CreateFont_();

	g_VGuiSurface->SetFontGlyphSet(esp_font, "Bell Gothic", 11, 700, 0, 0, FONTFLAG_DROPSHADOW);
	g_VGuiSurface->SetFontGlyphSet(injection_font, "Bell Gothic", 140, 300, 0, 0, FONTFLAG_OUTLINE);

	g_VGuiSurface->DrawSetTextFont(esp_font);

	return true;
}

void visuals_pt::destroy_fonts()
{

}

void visuals_pt::draw_3d_box(float scalar, QAngle angles, Vector middle_origin, Color outline)
{
		Vector forward, right, up;
		Math::AngleVectors(angles, forward, right, up);

		Vector points[8];
		points[0] = middle_origin - (right * scalar) + (up * scalar) - (forward * scalar); // BLT
		points[1] = middle_origin + (right * scalar) + (up * scalar) - (forward * scalar); // BRT
		points[2] = middle_origin - (right * scalar) - (up * scalar) - (forward * scalar); // BLB
		points[3] = middle_origin + (right * scalar) - (up * scalar) - (forward * scalar); // BRB

		points[4] = middle_origin - (right * scalar) + (up * scalar) + (forward * scalar); // FLT
		points[5] = middle_origin + (right * scalar) + (up * scalar) + (forward * scalar); // FRT
		points[6] = middle_origin - (right * scalar) - (up * scalar) + (forward * scalar); // FLB
		points[7] = middle_origin + (right * scalar) - (up * scalar) + (forward * scalar); // FRB

		Vector points_screen[8];
		for (int i = 0; i < 8; i++)
			if (!Math::WorldToScreen(points[i], points_screen[i]))
				return;

		g_VGuiSurface->DrawSetColor(outline);

		// Back frame
		g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[1].x, points_screen[1].y);
		g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[2].x, points_screen[2].y);
		g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[1].x, points_screen[1].y);
		g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[2].x, points_screen[2].y);

		// Frame connector
		g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[4].x, points_screen[4].y);
		g_VGuiSurface->DrawLine(points_screen[1].x, points_screen[1].y, points_screen[5].x, points_screen[5].y);
		g_VGuiSurface->DrawLine(points_screen[2].x, points_screen[2].y, points_screen[6].x, points_screen[6].y);
		g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[7].x, points_screen[7].y);

		// Front frame
		g_VGuiSurface->DrawLine(points_screen[4].x, points_screen[4].y, points_screen[5].x, points_screen[5].y);
		g_VGuiSurface->DrawLine(points_screen[4].x, points_screen[4].y, points_screen[6].x, points_screen[6].y);
		g_VGuiSurface->DrawLine(points_screen[7].x, points_screen[7].y, points_screen[5].x, points_screen[5].y);
		g_VGuiSurface->DrawLine(points_screen[7].x, points_screen[7].y, points_screen[6].x, points_screen[6].y);
}


void visuals_pt::Player::draw_bone_esp()
{
	C_BasePlayer* entity = esp_ctx.pl;

	if (!entity->GetModel())
		return;

	auto draw_bone_line = [=](int bone, int bone_parent) -> void
	{
		Vector bone_pos;
		Vector bone_parent_pos;

		if (!(Math::WorldToScreen(entity->get_bone_pos(bone), bone_pos)))
			return;

		if (!(Math::WorldToScreen(entity->get_bone_pos(bone_parent), bone_parent_pos)))
			return;

		g_VGuiSurface->DrawSetColor(esp_ctx.clr);
		g_VGuiSurface->DrawLine(bone_pos.x, bone_pos.y, bone_parent_pos.x, bone_parent_pos.y);
	};

	studiohdr_t* studio_model = g_MdlInfo->GetStudiomodel(entity->GetModel());

	if (!studio_model)
		return;

	for (int i = 0; i < studio_model->numbones; ++i)
	{
		mstudiobone_t* bone = studio_model->GetBone(i).value();
		if (!bone || !(bone->flags & BONE_USED_BY_HITBOX) || bone->parent == -1)
			continue;

		draw_bone_line(i, bone->parent);
	}
}

bool visuals_pt::Player::Begin(C_BasePlayer* pl)
{
	esp_ctx.pl = pl;

	if (!pl)
		return false;

	esp_ctx.right_text_size = 0;
	esp_ctx.bottom_text_size = 0;

	if (!pl->is_alive() || Player::is_completely_dormant())
		return false;

	if (!pl->m_hActiveWeapon())
		return false;

	esp_ctx.is_enemy = g_LocalPlayer->m_iTeamNum() != pl->m_iTeamNum();
	esp_ctx.is_team = g_LocalPlayer->m_iTeamNum() == pl->m_iTeamNum();
	esp_ctx.is_visible = g_LocalPlayer->CanSeePlayer(pl, HITBOX_CHEST);

	const bool local = g_LocalPlayer->EntIndex() == pl->EntIndex();

	if (esp_ctx.is_enemy && !local && !g_vars.esp_targets[1])
		return false;

	if (esp_ctx.is_team && !local && !g_vars.esp_targets[0])
		return false;

	esp_ctx.clr = esp_ctx.is_enemy ? (esp_ctx.is_visible ? g_vars.color_esp[1][0] : g_vars.color_esp[1][1]) : (esp_ctx.is_visible ? g_vars.color_esp[0][0] : g_vars.color_esp[0][1]);

	if (local)
		esp_ctx.clr = g_vars.color_esp[2][0];

	auto head = Player::get_best_origin() + Vector(0, 0, pl->GetCollideable()->OBBMaxs().z);
	auto origin = Player::get_best_origin();

	if (!Math::WorldToScreen(head, esp_ctx.head_pos) || !Math::WorldToScreen(origin, esp_ctx.feet_pos))
		return false;

	auto h = fabs(esp_ctx.head_pos.y - esp_ctx.feet_pos.y);
	auto w = h / 1.65f;

	esp_ctx.bbox.left = static_cast<long>(esp_ctx.feet_pos.x - w * 0.5f);
	esp_ctx.bbox.right = static_cast<long>(esp_ctx.bbox.left + w);
	esp_ctx.bbox.bottom = static_cast<long>(esp_ctx.feet_pos.y);
	esp_ctx.bbox.top = static_cast<long>(esp_ctx.head_pos.y);

	esp_ctx.pinfo = &pl->get_player_info();

	esp_ctx.is_vs_user = (std::find(info_lp::vs_users.begin(), info_lp::vs_users.end(), esp_ctx.pinfo->steamID64) != info_lp::vs_users.end());
	return true;
}

Vector visuals_pt::Player::get_best_origin()
{
	if (!esp_ctx.pl || !esp_ctx.pl->is_alive())
		return Vector{0, 0, 0}; //who let this happen

	if(esp_ctx.pl->is_dormant())
	{
		auto index = esp_ctx.pl->EntIndex();
		auto current_far_esp_player = far_esp::Get().sound_players[index];
		auto current_radar_player = c_radar_far_esp::radar_logs[index];
		if (!current_far_esp_player.dormant)
			return current_far_esp_player.origin;
		else if (current_radar_player.is_valid_log)
			return current_radar_player.origin;
		else
			return Vector{ 0,0,0 }; //oh god oh fuck
	}
	else
	{
		return esp_ctx.pl->m_vecOrigin();
	}
}

bool visuals_pt::Player::is_completely_dormant()
{
	if (!esp_ctx.pl || !esp_ctx.pl->is_alive())
		return true;

	auto index = esp_ctx.pl->EntIndex();
	auto current_far_esp_player = far_esp::Get().sound_players[index];
	auto current_radar_player = c_radar_far_esp::radar_logs[index];
	auto is_real_dormant = esp_ctx.pl->is_dormant();
	auto is_far_dormant = current_far_esp_player.dormant;
	auto is_radar_dormant = !current_radar_player.is_valid_log;

	if (!g_vars.esp_farther && is_real_dormant)
		return true;

	return is_real_dormant && is_far_dormant && is_radar_dormant;
}

void visuals_pt::Player::draw_vs_user()
{
	if (!esp_ctx.pl || !esp_ctx.pl->is_alive())
		return;

	if (!esp_ctx.pinfo)
		return;

	if (esp_ctx.is_vs_user)
	{
		draw_string(Color(238, 232, 170, 255), "V$ USER", esp_ctx.bbox.right + 5, esp_ctx.bbox.top + esp_ctx.right_text_size * 10);
		esp_ctx.right_text_size += 1;
	}
}

void visuals_pt::Player::render_box()
{

	if (!esp_ctx.pl || !esp_ctx.pl->is_alive())
		return;

	g_VGuiSurface->DrawSetColor(esp_ctx.clr);
	g_VGuiSurface->DrawOutlinedRect(esp_ctx.bbox.left, esp_ctx.bbox.top, esp_ctx.bbox.right, esp_ctx.bbox.bottom);
	Color black = Color(Color::Black.r(), Color::Black.g(), Color::Black.b(), esp_ctx.clr.a());
	g_VGuiSurface->DrawSetColor(black);
	g_VGuiSurface->DrawOutlinedRect(esp_ctx.bbox.left - 1, esp_ctx.bbox.top - 1, esp_ctx.bbox.right + 1, esp_ctx.bbox.bottom + 1);
	g_VGuiSurface->DrawOutlinedRect(esp_ctx.bbox.left + 1, esp_ctx.bbox.top + 1, esp_ctx.bbox.right - 1, esp_ctx.bbox.bottom - 1);
}

void visuals_pt::Player::render_name()
{
	wchar_t buf[256];

	player_info_t info = esp_ctx.pl->get_player_info();

	if (MultiByteToWideChar(CP_UTF8, 0, info.szName, -1, buf, 256) > 0) {
		int tw, th;
		g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(esp_font);
		g_VGuiSurface->DrawSetTextColor(esp_ctx.clr);
		g_VGuiSurface->DrawSetTextPos(esp_ctx.feet_pos.x - tw / 2, esp_ctx.head_pos.y - th);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
	}
}

void visuals_pt::Player::render_vulnerabilities()
{
	//ok epic lambda style
	auto dist = [=](Vector a, Vector b) -> int
	{
		return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
	};

	const int x = esp_ctx.bbox.right + 5;
	const int y = esp_ctx.bbox.top;

	int count = 0;
	auto draw_and_add = [&](const char* text) -> void
	{
		draw_string(esp_ctx.clr, text, x, y + count * 10);
		count++;
	};

	if (esp_ctx.pl->m_bHasHelmet() || esp_ctx.pl->m_ArmorValue() > 0)
		draw_and_add(esp_ctx.pl->m_bHasHelmet() ? (esp_ctx.pl->m_ArmorValue() > 0 ? "hk" : "h") : (esp_ctx.pl->m_ArmorValue() > 0 ? "k" : ""));

	if (esp_ctx.pl->m_hActiveWeapon()->m_iClip1() == 0 && !esp_ctx.pl->m_hActiveWeapon()->is_knife())
		draw_and_add("no ammo!");

	if (esp_ctx.pl->m_hActiveWeapon()->is_knife())
		draw_and_add("knife out!");

	if (esp_ctx.pl->m_hActiveWeapon()->is_grenade())
		draw_and_add("grenade out!");

	if (esp_ctx.pl->m_bIsScoped())
		draw_and_add("scoped!");

	if (esp_ctx.pl->GetSequenceActivity(esp_ctx.pl->GetAnimOverlay(1).value()->m_nSequence) == 967 && esp_ctx.pl->GetAnimOverlay(1).value()->m_flWeight != 0)
		draw_and_add("reloading!");

	/*for (int i = 0; i < esp_ctx.pl->GetNumAnimOverlays(); i++)
	{
		auto seq_anim = esp_ctx.pl->GetSequenceActivity(esp_ctx.pl->GetAnimOverlay(i)->m_nSequence);
		float seq_weight = esp_ctx.pl->GetAnimOverlay(i)->m_flWeight;
		if (seq_anim == 978 && seq_weight != 0)
		{
			draw_and_add("planting!");
		}

		if ((seq_anim == 958 || seq_anim == 959) && seq_weight != 0)
		{
			draw_and_add("defusing!");
		}
	}*/

	const int dist_from_local = dist(g_LocalPlayer->m_vecOrigin(), esp_ctx.pl->m_vecOrigin());

	if (dist_from_local < 183.f)
		draw_and_add("lethal taser range!");

	if (dist_from_local < 49.f)
		draw_and_add(dist_from_local <= 32.f ? "knife stab range!" : "knife slash range!");

	if (esp_ctx.pl->is_flashed())
		draw_and_add("flashed!");

	esp_ctx.right_text_size = count;
}

void visuals_pt::Player::render_health()
{

	//for dz / community shitty fix
	const auto hp = esp_ctx.pl->m_iHealth() > 100 ? 100 : esp_ctx.pl->m_iHealth();

	if (!hp)
		return;

	const float box_h = (float)fabs(esp_ctx.bbox.bottom - esp_ctx.bbox.top);

	const float off = 5 + (g_vars.esp_player_armour ? g_vars.esp_player_armour_thick : 0) + g_vars.esp_player_health_thick;

	const auto height = (box_h * hp) / 100;

	const int green = int(hp * 2.55f);
	const int red = 255 - green;

	const int x = esp_ctx.bbox.left - off;
	const int y = esp_ctx.bbox.top;
	const int w = g_vars.esp_player_health_thick + 1;
	const int h = box_h;
	const int line = box_h / 10;

	Color black = Color(Color::Black.r(), Color::Black.g(), Color::Black.b(), esp_ctx.clr.a());
	g_VGuiSurface->DrawSetColor(black);
	g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);

	g_VGuiSurface->DrawSetColor(Color(red, green, 0, esp_ctx.clr.a()));
	g_VGuiSurface->DrawFilledRect(x + 1, y + 1, x + w - 1, y + height - 1);

	if (g_vars.esp_player_health_fractional)
	{
		g_VGuiSurface->DrawSetColor(black);
		for (int i = 0; i < 10; i++)
		{
			g_VGuiSurface->DrawLine(x, y + i * line, x + w - 1, y + i * line);
		}
	}
}

void visuals_pt::Player::render_armor()
{

	const auto armour = esp_ctx.pl->m_ArmorValue();

	if (!armour)
		return;

	const float box_h = (float)fabs(esp_ctx.bbox.bottom - esp_ctx.bbox.top);

	const float off = 3 + g_vars.esp_player_armour_thick;

	const auto height = (box_h * armour) / 100;

	const int x = esp_ctx.bbox.left - off;
	const int y = esp_ctx.bbox.top;
	const int w = g_vars.esp_player_armour_thick + 1;
	const int h = box_h;
	const int line = box_h / 10;

	Color black = Color(Color::Black.r(), Color::Black.g(), Color::Black.b(), esp_ctx.clr.a());
	g_VGuiSurface->DrawSetColor(black);
	g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);

	g_VGuiSurface->DrawSetColor(Color(0, 50, 255, esp_ctx.clr.a()));
	g_VGuiSurface->DrawFilledRect(x + 1, y + 1, x + w - 1, y + height - 1);
	
	if (g_vars.esp_player_armour_fractional)
	{
		g_VGuiSurface->DrawSetColor(black);
		for (int i = 0; i < 10; i++)
		{
			g_VGuiSurface->DrawLine(x, y + i * line, x + w - 1, y + i * line);
		}
	}
}

void visuals_pt::Player::render_weapon()
{
	wchar_t buf[80];

	auto weapon = esp_ctx.pl->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	if (MultiByteToWideChar(CP_UTF8, 0, weapon->get_cs_wpn_data()->weapon_name + 7, -1, buf, 80) > 0) {
		int tw, th;
		g_VGuiSurface->GetTextSize(esp_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(esp_font);
		g_VGuiSurface->DrawSetTextColor(esp_ctx.clr);
		g_VGuiSurface->DrawSetTextPos(esp_ctx.feet_pos.x - tw / 2, esp_ctx.feet_pos.y);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
	}

	esp_ctx.bottom_text_size++;
}

void visuals_pt::Player::render_ammo_bar()
{
	auto weapon = esp_ctx.pl->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	int clip = weapon->m_iClip1();
	int max_clip = weapon->get_cs_wpn_data()->max_clip;

	if (clip < 0 || max_clip <= 0)
		return;

	const int left = esp_ctx.bbox.left;
	const int right = esp_ctx.bbox.right;
	const float percent = (float)clip / (float)max_clip;
	const auto black = Color(0, 0, 0, 130);
	const int y = esp_ctx.feet_pos.y + (esp_ctx.bottom_text_size * 11);

	//outline
	g_VGuiSurface->DrawSetColor(black);
	g_VGuiSurface->DrawFilledRect(left, y, right, y + 4);

	const auto width = (esp_ctx.bbox.right - esp_ctx.bbox.left) - 1;
	g_VGuiSurface->DrawSetColor(g_vars.color_ammo_bar);
	g_VGuiSurface->DrawFilledRect(left + 1, y + 1, left + width * (percent), y + 3);

	esp_ctx.bottom_text_size++;
}

void visuals_pt::Player::render_snapline()
{
	int screen_w, screen_h;
	g_EngineClient->GetScreenSize(screen_w, screen_h);

	g_VGuiSurface->DrawSetColor(esp_ctx.clr);

	g_VGuiSurface->DrawLine(
		screen_w / 2,
		screen_h,
		esp_ctx.feet_pos.x,
		esp_ctx.feet_pos.y);
}

void visuals_pt::Misc::render_crosshair()
{
	int w, h;

	g_EngineClient->GetScreenSize(w, h);

	g_VGuiSurface->DrawSetColor(g_vars.color_esp_crosshair);

	int cx = w / 2;
	int cy = h / 2;

	g_VGuiSurface->DrawLine(cx - 25, cy, cx + 25, cy);
	g_VGuiSurface->DrawLine(cx, cy - 25, cx, cy + 25);
}

void visuals_pt::Misc::render_planted_c4(C_BaseEntity* ent)
{
	auto head =  ent->m_vecOrigin() +  Vector(0, 0, ent->GetCollideable()->OBBMaxs().z);
	auto origin = ent->m_vecOrigin();

	Vector head_pos, feet_pos;

	if (!(Math::WorldToScreen(head, head_pos) || Math::WorldToScreen(origin, feet_pos)))
		return;

	auto h = fabs(head_pos.y - feet_pos.y);
	auto w = h / 1.65f;

	auto left = static_cast<long>(feet_pos.x - w * 0.5f);
	auto right = static_cast<long>(left + w);
	auto bottom = static_cast<long>(feet_pos.y);
	auto top = static_cast<long>(head_pos.y);

	int bomb_timer = std::ceil(ent->m_flC4Blow() - g_GlobalVars->curtime);
	std::string timer = "Bomb: " + std::to_string(bomb_timer);

	draw_string(g_vars.color_esp_c4, timer.c_str(), right, top);
}

void visuals_pt::Misc::spectator_list()
{
	if (!g_vars.misc_spec_list)
		return;

	int count = 0;

	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* cur_entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
		player_info_t p_info;

		if (!cur_entity || cur_entity == g_LocalPlayer)
			continue;

		if (!g_EngineClient->get_player_info(i, &p_info) || cur_entity->is_dormant())
			continue;
		
		if (p_info.fakeplayer || p_info.ishltv)
			continue;

		auto obs = cur_entity->m_hObserverTarget();

		if (!obs || obs != g_LocalPlayer)
			continue;

		draw_string(Color(255, 255, 255), p_info.szName, 10, 10 + (count * 10) + info_lp::spec_list_size);
		count++;
	}
	info_lp::spec_list_size += count * 10;
}


void visuals_pt::Misc::third_person()
{
	if (!g_LocalPlayer)
		return;

	static bool toggle_tp = false;
	
	if (!g_vars.misc_thirdperson)
	{
		if (!toggle_tp)
		{
			g_Input->m_fCameraInThirdPerson = false;
			toggle_tp = true;
		}
		return;
	}
	toggle_tp = false;

	static bool do_thirdperson = false;
	static long long press_time;

	const auto epoch_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	if (g_vars.misc_thirdperson_key > 0 && GetAsyncKeyState(g_vars.misc_thirdperson_key) && epoch_time - press_time > 500)
	{
		press_time = epoch_time;
		do_thirdperson = !do_thirdperson;
	}

	if (g_LocalPlayer->is_alive() && (do_thirdperson || g_vars.misc_thirdperson_key <= 0))
	{
		if (!g_Input->m_fCameraInThirdPerson)
		{
			g_Input->m_vecCameraOffset.roll = g_vars.misc_thirdperson_dist;
			g_Input->m_fCameraInThirdPerson = true;
		}
	}
	else
	{
		g_Input->m_vecCameraOffset.roll = 0;
		g_Input->m_fCameraInThirdPerson = false;
	}
}

void visuals_pt::Misc::draw_logo_bar()
{

	auto get_time_str = [=]() -> std::string
	{
		time_t current_time;
		struct tm *time_info;
		static char time_string[10];
		time(&current_time);
		time_info = localtime(&current_time);
		strftime(time_string, sizeof(time_string), "%X", time_info);
		return time_string;
	};

	auto char_to_wchar = [=](const char* c)
	{
		wchar_t formatted[2048] = { '\0' };
		wsprintfW(formatted, L"%S", c);
		return formatted;
	};

	auto draw_str = [=](vgui::HFont f, Color col, const char* str, int x, int y) -> void
	{
		wchar_t formatted[512] = { '\0' };
		wsprintfW(formatted, L"%S", str);
		g_VGuiSurface->DrawSetTextFont(f);
		g_VGuiSurface->DrawSetTextPos(x, y);
		g_VGuiSurface->DrawSetTextColor(col);
		g_VGuiSurface->DrawPrintText(formatted, wcslen(formatted));
	};

	int width, height;
	g_EngineClient->GetScreenSize(width, height);

	std::string info = "VER$ACE | " + get_time_str();
	const static Color text_clr = Color(155, 155, 155, 200);
	const static Color box_clr = Color(255, 153, 153, 35);
	//const static Color line_clr = Color(51, 153, 255, 140);

	int wide, tall;
	g_VGuiSurface->GetTextSize(esp_font, char_to_wchar(info.c_str()), wide, tall);

	g_VGuiSurface->DrawSetColor(box_clr);
	g_VGuiSurface->DrawFilledRect(width - wide, 9, width, 9 + tall);

	//g_VGuiSurface->DrawSetColor(line_clr);
	//g_VGuiSurface->DrawFilledRect(width - wide, 10, width, 12);

	draw_str(esp_font, text_clr, info.c_str(), width - wide, 11);
}

void visuals_pt::Misc::draw_aimbot_target()
{
	if (!g_vars.esp_aimbot_target)
		return;

	if (info_lp::current_target_index < 0 || info_lp::current_target_index > 64)
		return;

	C_BasePlayer* current_target = (C_BasePlayer*)g_EntityList->GetClientEntity(info_lp::current_target_index);

	if (!current_target || !current_target->is_alive() || current_target->is_dormant())
		return;

	int width, height;
	g_EngineClient->GetScreenSize(width, height);

	width /= 2;
	height /= 2;

	player_info_t info = current_target->get_player_info();

	std::string f_str = "currently targetting: ";
	f_str += info.szName;

	draw_string(g_vars.color_aimbot_target_text, f_str.c_str(), width - 20, height - 25);
}

void visuals_pt::Misc::draw_spread_crosshair()
{
	if (!g_vars.vis_misc_spread_crosshair)
		return;

	C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon();

	if (!weapon)
		return;

	int sw, sh;
	g_EngineClient->GetScreenSize(sw, sh);
	int cross_x = sw / 2;
	int cross_y = sh / 2;

	float recoil_step = sh / 180;

	cross_x -= (int)(g_LocalPlayer->m_aimPunchAngle().yaw * recoil_step);
	cross_y += (int)(g_LocalPlayer->m_aimPunchAngle().pitch * recoil_step);

	weapon->update_accuracy();
	float inaccuracy = weapon->get_accuracy();
	float spread = weapon->get_spread();

	float cone = inaccuracy * spread;
	cone *= sh * 0.7f;
	cone *= 90.f / 180;

	const static auto random_float = [=](float min, float max) -> float
	{
		typedef float(*random_float_t)(float, float);
		static random_float_t rand_float = (random_float_t)GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat");
		return rand_float(min, max);
	};

	for (int seed{ }; seed < 128; ++seed) {

		float    rand_a = random_float(0.f, 1.f);
		float    pi_rand_a = random_float(0.f, 2.f * M_PI);
		float    rand_b = random_float(0.0f, 1.f);
		float    pi_rand_b = random_float(0.f, 2.f * M_PI);

		float spread_x = cos(pi_rand_a) * (rand_a * inaccuracy) + cos(pi_rand_b) * (rand_b * spread);
		float spread_y = sin(pi_rand_a) * (rand_a * inaccuracy) + sin(pi_rand_b) * (rand_b * spread);

		float max_x = cos(pi_rand_a) * cone + cos(pi_rand_b) * cone;
		float max_y = sin(pi_rand_a) * cone + sin(pi_rand_b) * cone;

		float step = sh / 180 * 90.f;
		int screen_spread_x = (int)(spread_x * step * 0.7f);
		int screen_spread_y = (int)(spread_y * step * 0.7f);

		float percentage = (rand_a * inaccuracy + rand_b * spread) / (inaccuracy + spread);

		g_VGuiSurface->DrawSetColor(g_vars.color_spread_crosshair);
		g_VGuiSurface->DrawFilledRect(cross_x + screen_spread_x, cross_y + screen_spread_y, cross_x + screen_spread_x + 1, cross_y + screen_spread_y + 1);
	}
}

void visuals_pt::Misc::draw_taser_range()
{
	if (!g_vars.vis_misc_taser_range)
		return;

	if (!g_LocalPlayer || !g_LocalPlayer->is_alive())
		return; 

	C_BaseCombatWeapon *local_weapon = g_LocalPlayer->m_hActiveWeapon();

	if (!local_weapon)
		return;

	if (local_weapon->m_iItemDefinitionIndex() != WEAPON_TASER)
		return;

	if (!g_Input->m_fCameraInThirdPerson)
		return;

	Vector prev_scr_pos, scr_pos;

	constexpr float step = M_PI * 2.0 / 512;
	constexpr float radius = 178.f;
	Vector origin = g_LocalPlayer->get_eye_pos();
	origin.z -= 10;

	for (float rotation = 0; rotation <= (M_PI * 2.0) + step; rotation += step)
	{
		Vector pos(radius * cos(rotation) + origin.x, radius * sin(rotation) + origin.y, origin.z);

		g_VGuiSurface->DrawSetColor(g_vars.color_taser_range);

		Ray_t ray;
		trace_t trace;
		CTraceFilterWorldAndPropsOnly filter;

		ray.Init(origin, pos);

		g_EngineTrace->TraceRay(ray, CONTENTS_SOLID | CONTENTS_WINDOW | CONTENTS_MOVEABLE, &filter, &trace);
		if (Math::WorldToScreen(trace.endpos, scr_pos))
		{
			if (prev_scr_pos.IsValid())
			{
				g_VGuiSurface->DrawLine(prev_scr_pos.x, prev_scr_pos.y, scr_pos.x, scr_pos.y);
			}
			prev_scr_pos = scr_pos;
		}
	}
}

void visuals_pt::Misc::draw_autowall_damage()
{
	if (!g_vars.vis_misc_autowall_crosshair)
		return;

	auto can_wallbang = [](float &dmg) -> bool
	{
		auto local = g_LocalPlayer;
		if (!local)
			return false;

		FireBulletData data = FireBulletData(local->get_eye_pos());
		data.filter = CTraceFilter();
		data.filter.pSkip = local;

		QAngle eye_ang;
		g_EngineClient->GetViewAngles(&eye_ang);

		Vector dst, forward;

		Math::AngleVectors(eye_ang, forward);
		dst = data.src + (forward * 8196.f);

		QAngle angles;
		angles = Math::CalcAngle(data.src, dst);
		Math::AngleVectors(angles, data.direction);
		data.direction = data.direction.Normalized();

		C_BaseCombatWeapon* weapon = local->m_hActiveWeapon().Get();

		if (!weapon)
			return false;

		data.penetrate_count = 1;
		data.trace_length = 0.0f;

		CCSWeapData *weapon_data = weapon->get_cs_wpn_data();

		if (!weapon_data)
			return false;

		data.current_damage = (float)weapon_data->damage;

		data.trace_length_remaining = weapon_data->range - data.trace_length;

		Vector end = data.src + data.direction * data.trace_length_remaining;

		c_autowall::Get().trace_line(data.src, end, MASK_SHOT | CONTENTS_GRATE, local, &data.enter_trace);

		if (data.enter_trace.fraction == 1.0f)
			return false;

		if (c_autowall::Get().handle_bullet_penetration(weapon_data, data))
		{
			dmg = data.current_damage;
			return true;
		}

		return false;
	};

	auto char_to_wchar = [=](const char* c)
	{
		wchar_t formatted[2048] = { '\0' };
		wsprintfW(formatted, L"%S", c);
		return formatted;
	};

	float damage = 0.f;
	if (can_wallbang(damage))
	{
		//ceilf cuz im too lazy for rounding properly
		std::string dmg = std::to_string((int)damage) + " dmg";

		int string_wide, string_tall;
		g_VGuiSurface->GetTextSize(esp_font, char_to_wchar(dmg.c_str()), string_wide, string_tall);

		int wide, tall;
		g_EngineClient->GetScreenSize(wide, tall);

		wide /= 2;
		tall /= 2;

		draw_string(g_vars.color_autowall_crosshair, dmg.c_str(), wide - (string_wide / 2), tall - 18);
	}
}


void visuals_pt::Misc::draw_hud_features()
{
	if (!g_vars.vis_misc_hud_features)
		return;

	std::vector<std::string> features_enabled;

	auto char_to_wchar = [=](const char* c)
	{
		wchar_t formatted[2048] = { '\0' };
		wsprintfW(formatted, L"%S", c);
		return formatted;
	};

	if (g_vars.aimbot_enabled)
	{
		std::string aimbot_info = "aimbot: " + std::to_string(std::round(g_vars.aimbot_fov)) + " " + std::to_string(g_vars.aimbot_smooth);
		features_enabled.push_back(aimbot_info);
	}

	if (g_vars.esp_enabled)
	{
		std::string esp_info = "esp: ";
		if (g_vars.esp_player_boxes)
			esp_info += "box ";

		if (g_vars.esp_player_health)
			esp_info += "health ";

		if (g_vars.esp_player_names)
			esp_info += "name ";

		features_enabled.push_back(esp_info);
	}

	if (g_vars.misc_bhop)
	{
		std::string jump_info = "bhop: ";

		if (g_vars.misc_autostrafe)
			jump_info += "autostrafe";

		features_enabled.push_back(jump_info);
	}

	if (g_vars.misc_legit_aa)
	{
		std::string aa_info = "legit aa: " + std::to_string(g_vars.misc_legit_aa_ang);

		if (g_vars.misc_legit_aa_break_lby)
			aa_info += " lby: " + std::to_string(g_vars.misc_legit_aa_break_lby_dist);

		features_enabled.push_back(aa_info);
	}

	for (int i = 0; i < features_enabled.size(); i++)
	{
		auto cur_feature = features_enabled.at(i).c_str();
		constexpr int indent = 10; //x val
		const int y = 10 + (i * 10) + info_lp::spec_list_size;
		if (g_vars.vis_misc_hud_features_box)
		{
			int w, h;
			g_VGuiSurface->GetTextSize(esp_font, char_to_wchar(cur_feature), w, h);
			g_VGuiSurface->DrawSetColor(g_vars.color_hud_feature_box);
			g_VGuiSurface->DrawFilledRect(indent - 1, y, indent + w, y + h);
		}

		draw_string(g_vars.color_hud_feature_text, cur_feature, indent, y);
	}
	//+1 is ghetto fix because this didnt work with hurt notif
	info_lp::spec_list_size += (features_enabled.size() + 1) * 10;
}

void visuals_pt::Misc::draw_fov_ring()
{
	if (!g_vars.aimbot_enabled)
		return;

	if (!g_vars.vis_misc_fov_ring)
		return;

	int x;
	int y;
	g_EngineClient->GetScreenSize(x, y);

	Vector center;
	center.x = x / 2;
	center.y = y / 2;

	constexpr auto player_fov = 97;
	auto radius = tanf((DEG2RAD(g_vars.aimbot_fov)) / 6) / tanf(player_fov) * x;
	g_VGuiSurface->DrawSetColor(g_vars.color_fov_ring);
	g_VGuiSurface->DrawOutlinedCircle(center.x, center.y, radius, g_vars.aimbot_fov * 20);
}

void visuals_pt::Misc::draw_intro()
{
	//looks like shit, fix font
	//also doesnt fade out properly

	static long long start_time = Utils::get_epoch_time();

	if (Utils::get_epoch_time() - start_time > 3000)
		return;

	float percent = (Utils::get_epoch_time() - start_time) / 3000;

	auto draw_title_str = [](Color col, const char* str, int x, int y) -> void
	{
		wchar_t formatted[512] = { '\0' };
		wsprintfW(formatted, L"%S", str);
		g_VGuiSurface->DrawSetTextFont(injection_font);
		g_VGuiSurface->DrawSetTextPos(x, y);
		g_VGuiSurface->DrawSetTextColor(col);
		g_VGuiSurface->DrawPrintText(formatted, wcslen(formatted));
	};

	int width, height;
	g_EngineClient->GetScreenSize(width, height);

	width /= 2;
	height /= 2;

	draw_title_str(Color(255, 255, 255, (int)(255 - (percent * 255))), "VER$ACE", width, height);
}

void visuals_pt::Misc::draw_dangerzone_items(C_BaseEntity* ent)
{
	if (!ent)
		return;

	std::string item_str = "unknown item";
	const model_t* item_mdl = ent->GetModel();

	if (!item_mdl)
		return;

	studiohdr_t* hdr = g_MdlInfo->GetStudiomodel(item_mdl);

	if (!hdr)
		return;

	item_str = hdr->szName;
	if (ent->GetClientClass()->m_ClassID == ClassId_CBumpMine)
		item_str = "";
	else if (item_str.find("case_pistol") != std::string::npos) //convert these into class id for optimization
		item_str = "pistol case";
	else if (item_str.find("case_light_weapon") != std::string::npos)
		item_str = "light weapon case";
	else if (item_str.find("case_heavy_weapon") != std::string::npos)
		item_str = "heavy weapon case";
	else if (item_str.find("case_explosive") != std::string::npos)
		item_str = "explosive case";
	else if (item_str.find("case_tools") != std::string::npos)
		item_str = "tools case";
	else if (item_str.find("random") != std::string::npos)
		item_str = "airdrop";
	else if (item_str.find("dz_armor_helmet") != std::string::npos)
		item_str = "full armor";
	else if (item_str.find("dz_helmet") != std::string::npos)
		item_str = "helmet";
	else if (item_str.find("dz_armor") != std::string::npos)
		item_str = "armor";
	else if (item_str.find("upgrade_tablet") != std::string::npos)
		item_str = "tablet upgrade";
	else if (item_str.find("briefcase") != std::string::npos)
		item_str = "briefcase";
	else if (item_str.find("parachutepack") != std::string::npos)
		item_str = "parachute";
	else if (item_str.find("dufflebag") != std::string::npos)
		item_str = "$";
	else if (item_str.find("ammobox") != std::string::npos)
		item_str = "ammobox";
	else if (item_str.find("dronegun") != std::string::npos)
		item_str = "turret";
	else if (item_str.find("exojump") != std::string::npos)
		item_str = "exojump";
	else if (item_str.find("healthshot") != std::string::npos)
		item_str = "healthshot";
	else
		return;

	int f_w, f_h;

	auto item_str_w = std::wstring(item_str.begin(), item_str.end());
	g_VGuiSurface->GetTextSize(esp_font, item_str_w.c_str(), f_w, f_h);

	Vector w2s;
	if (!Math::WorldToScreen(ent->m_vecOrigin(), w2s))
		return;

	draw_string(g_vars.color_dangerzone_items, item_str.c_str(), (w2s.x) + f_w * 0.5f, w2s.y + 1);
}

