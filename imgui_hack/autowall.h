#pragma once
#include "valve_sdk/csgostructs.hpp"
#include "singleton.hpp"


struct FireBulletData
{
	FireBulletData(const Vector& eye_pos)
		: src(eye_pos)
	{
	}

	Vector src;
	trace_t enter_trace;
	Vector direction;
	CTraceFilter filter;
	float trace_length;
	float trace_length_remaining;
	float current_damage;
	int penetrate_count;
};

class c_autowall : public Singleton<c_autowall>
{
public:
	float can_hit(Vector& point);
	void trace_line(Vector& abs_start, Vector& abs_end, unsigned int mask, IClientEntity* ignore, CGameTrace* ptr);
	bool handle_bullet_penetration(CCSWeapData* wpn_data, FireBulletData& data);
	Vector calculate_best_point(C_BasePlayer* entity, int prioritized, float min_dmg, bool only_prioritized);
	void clip_trace_to_players(const Vector& abs_start, Vector abs_end, unsigned int mask, ITraceFilter* filter,
	                           CGameTrace* tr);
private:
	bool breakable_entity(IClientEntity* entity);
	void scale_damage(CGameTrace& enter_trace, CCSWeapData* weapon_data, float& current_damage);
	bool trace_to_exit(CGameTrace& enter_trace, CGameTrace& exit_trace, Vector start_pos, Vector direction);
	bool simulate_fire_bullet(C_BaseCombatWeapon* weapon, FireBulletData& data);
	float best_hit_point(C_BasePlayer* entity, int prioritized, float min_dmg, Vector& out);
};
