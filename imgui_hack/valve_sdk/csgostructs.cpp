#include "csgostructs.hpp"
#include "../Helpers/Math.hpp"
#include "../Helpers/Utils.hpp"
#include "../player_matrix.h"
#include <optional>

bool C_BaseEntity::IsPlayer()
{
	//index: 152
	//ref: "effects/nightvision"
	//sig: 8B 92 ? ? ? ? FF D2 84 C0 0F 45 F7 85 F6
	return call_vfunc<bool(__thiscall*)(C_BaseEntity*)>(this, 157)(this);
}

bool C_BaseEntity::IsWeapon()
{
	//index: 160
	//ref: "CNewParticleEffect::DrawModel"
	//sig: 8B 80 ? ? ? ? FF D0 84 C0 74 6F 8B 4D A4

	auto v1 = call_vfunc<bool(__thiscall*)(C_BaseEntity*)>(this, 165)(this);
	auto v2 = this->GetClientClass()->m_ClassID >= ClassId_CWeaponAug && this->GetClientClass()->m_ClassID <= ClassId_CWeaponXM1014;
	auto v3 = this->GetClientClass()->m_ClassID == ClassId_CAK47 || this->GetClientClass()->m_ClassID == ClassId_CDEagle || this->GetClientClass()->m_ClassID == ClassId_CSCAR17;
	auto v4 = strstr(this->GetClientClass()->m_pNetworkName, "Weapon");
	return v1 || v2 || v3 || v4;
}


bool C_BaseEntity::IsPlantedC4()
{
	return GetClientClass()->m_ClassID == ClassId::ClassId_CPlantedC4;
}

bool C_BaseEntity::is_defuse_kit()
{
	return GetClientClass()->m_ClassID == ClassId::ClassId_CBaseAnimating;
}

bool C_BaseEntity::IsLoot() {
	return GetClientClass()->m_ClassID == ClassId_CPhysPropAmmoBox ||
		GetClientClass()->m_ClassID == ClassId_CPhysPropLootCrate ||
		GetClientClass()->m_ClassID == ClassId_CPhysPropRadarJammer ||
		GetClientClass()->m_ClassID == ClassId_CPhysPropWeaponUpgrade ||
		GetClientClass()->m_ClassID == ClassId_CDrone ||
		GetClientClass()->m_ClassID == ClassId_CDronegun ||
		GetClientClass()->m_ClassID == ClassId_CItem_Healthshot ||
		GetClientClass()->m_ClassID == ClassId_CItemCash ||
		GetClientClass()->m_ClassID == ClassId_CBumpMine;
}

CCSWeapData* C_BaseCombatWeapon::get_cs_wpn_data()
{
	return call_vfunc<CCSWeapData*(__thiscall*)(void*)>(this, 457)(this);
}

bool C_BaseCombatWeapon::has_bullets()
{
	if (!this)
		return false;

	return !(is_reloading()) && (m_iClip1() > 0);
}

bool C_BaseCombatWeapon::can_fire()
{
	if (!this)
		return false;

	if (is_reloading() || m_iClip1() <= 0)
		return false;

	if (!g_LocalPlayer)
		return false;

	float flServerTime = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;

	return m_flNextPrimaryAttack() <= flServerTime;
}

bool C_BaseCombatWeapon::is_grenade()
{
	return get_cs_wpn_data()->type == WEAPONTYPE_GRENADE;
}

bool C_BaseCombatWeapon::is_gun()
{
	switch (get_cs_wpn_data()->type)
	{
	case WEAPONTYPE_C4:
		return false;
	case WEAPONTYPE_GRENADE:
		return false;
	case WEAPONTYPE_KNIFE:
		return false;
	case WEAPONTYPE_UNKNOWN:
		return false;
	default:
		return true;
	}
}

bool C_BaseCombatWeapon::is_knife()
{
	if (this->m_iItemDefinitionIndex() == WEAPON_TASER)
		return false;

	return get_cs_wpn_data()->type == WEAPONTYPE_KNIFE;
}

bool C_BaseCombatWeapon::is_taser()
{
	return this->m_iItemDefinitionIndex() == WEAPON_TASER;
}


bool C_BaseCombatWeapon::is_rifle()
{
	switch (get_cs_wpn_data()->type)
	{
	case WEAPONTYPE_RIFLE:
		return true;
	case WEAPONTYPE_SUBMACHINEGUN:
		return true;
	case WEAPONTYPE_SHOTGUN:
		return true;
	case WEAPONTYPE_MACHINEGUN:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::is_pistol()
{
	switch (get_cs_wpn_data()->type)
	{
	case WEAPONTYPE_PISTOL:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::is_sniper()
{
	short id = this->m_iItemDefinitionIndex();

	switch (id)
	{
	case ItemDefinitionIndex::WEAPON_AUG:
	case ItemDefinitionIndex::WEAPON_AWP:
	case ItemDefinitionIndex::WEAPON_G3SG1:
	case ItemDefinitionIndex::WEAPON_SG556:
	case ItemDefinitionIndex::WEAPON_SSG08:
	case ItemDefinitionIndex::WEAPON_SCAR20:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::is_reloading()
{
	static auto inReload = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90").value() + 2);
	return *(bool*)((uintptr_t)this + inReload);
}

CUtlVector<IRefCounted*>& C_BaseCombatWeapon::m_CustomMaterials()
{
	static auto custom_mats = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "83 BE ? ? ? ? ? 7F 67").value() + 2) - 12;
	return *(CUtlVector<IRefCounted*>*)((uintptr_t)this + custom_mats);
}

bool* C_BaseCombatWeapon::m_bCustomMaterialInitialized()
{
	static auto custom_mat_init = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "C6 86 ? ? ? ? ? FF 50 04").value() + 2);
	return (bool*)((uintptr_t)this + custom_mat_init);
}

float C_BaseCombatWeapon::get_spread()
{
		typedef float(__thiscall* OriginFN)(void*);
		return call_vfunc<OriginFN>(this, 449)(this);
}

void C_BaseCombatWeapon::update_accuracy()
{
		typedef void(__thiscall* OriginFN)(void*);
		call_vfunc<OriginFN>(this, 480)(this);
}

CUserCmd*& C_BasePlayer::m_pCurrentCommand()
{
	static auto currentCommand = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "89 BE ? ? ? ? E8 ? ? ? ? 85 FF").value() + 2);
	return *(CUserCmd**)((uintptr_t)this + currentCommand);
}

int C_BasePlayer::GetNumAnimOverlays()
{
	return *(int*)((DWORD)this + 0x298C);
}

AnimationLayer *C_BasePlayer::GetAnimOverlays()
{
	// to find offset: use 9/12/17 dll
	// sig: 55 8B EC 51 53 8B 5D 08 33 C0
	return *(AnimationLayer**)((DWORD)this + 0x2980);
}

std::optional<AnimationLayer*> C_BasePlayer::GetAnimOverlay(int i)
{
	if (i < 15)
		return &GetAnimOverlays()[i];
	return {};
}

int C_BasePlayer::GetSequenceActivity(int sequence)
{
	auto hdr = g_MdlInfo->GetStudiomodel(this->GetModel());

	if (!hdr)
		return -1;

	// sig for stuidohdr_t version: 53 56 8B F1 8B DA 85 F6 74 55
	// sig for C_BaseAnimating version: 55 8B EC 83 7D 08 FF 56 8B F1 74 3D
	// c_csplayer vfunc 242, follow calls to find the function.

	static auto get_sequence_activity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(Utils::PatternScan(GetModuleHandle(L"client_panorama.dll"), "55 8B EC 53 8B 5D 08 56 8B F1 83").value());

	return get_sequence_activity(this, hdr, sequence);
}

c_baseplayer_animstate *C_BasePlayer::get_base_player_animstate()
{
	return *(c_baseplayer_animstate**)((DWORD)this + 0x3900);
}

ccs_player_animstate *C_BasePlayer::get_player_animstate()
{
	return *(ccs_player_animstate**)((DWORD)this + 0x3870);
}

void C_BasePlayer::UpdateAnimationState(c_baseplayer_animstate*state, QAngle angle)
{
	static auto UpdateAnimState = Utils::PatternScan(
		GetModuleHandle(L"client_panorama.dll"), "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3").value();
	if (!UpdateAnimState)
		return;

	__asm {
		push 0
	}

	__asm
	{
		mov ecx, state

		movss xmm1, dword ptr[angle + 4]
		movss xmm2, dword ptr[angle]

		call UpdateAnimState
	}
}

void C_BasePlayer::ResetAnimationState(c_baseplayer_animstate* state)
{
	using ResetAnimState_t = void(__thiscall*)(c_baseplayer_animstate*);
	static auto ResetAnimState = (ResetAnimState_t)(Utils::PatternScan(GetModuleHandle(L"client_panorama.dll"), "56 6A 01 68 ? ? ? ? 8B F1").value());
	if (!ResetAnimState)
		return;

	ResetAnimState(state);
}

void C_BasePlayer::CreateAnimationState(c_baseplayer_animstate* state)
{
	using CreateAnimState_t = void(__thiscall*)(c_baseplayer_animstate*, C_BasePlayer*);
	static auto CreateAnimState = (CreateAnimState_t)(Utils::PatternScan(GetModuleHandle(L"client_panorama.dll"), "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46").value());
	if (!CreateAnimState)
		return;

	CreateAnimState(state, this);
}

Vector C_BasePlayer::get_eye_pos()
{
	return m_vecOrigin() + m_vecViewOffset();
}

player_info_t C_BasePlayer::get_player_info()
{
	player_info_t info;
	g_EngineClient->get_player_info(EntIndex(), &info);
	return info;
}

bool C_BasePlayer::is_alive()
{
	return m_lifeState() == LIFE_ALIVE && m_iHealth() > 0;
}

bool C_BasePlayer::is_flashed()
{
	if (m_flFlashDuration() > 0.f) 
		return true;

	return false;
}

bool C_BasePlayer::HasC4()
{
	static auto fnHasC4
		= reinterpret_cast<bool(__thiscall*)(void*)>(
			Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "56 8B F1 85 F6 74 31").value()
			);

	return fnHasC4(this);
}

Vector C_BasePlayer::get_backtracked_hitbox_pos(int hitbox_id, matrixes m)
{
	if (!this)
		return Vector{};

	if (!this->is_alive())
		return Vector{};

	if (!GetModel())
		return Vector{};

	auto studio_model = g_MdlInfo->GetStudiomodel(GetModel());
	if (!studio_model)
		return Vector{};

	auto hitbox = studio_model->GetHitboxSet(0).value()->GetHitbox(hitbox_id).value();
	if (!hitbox)
		return Vector{};

	auto min = Vector{}, max = Vector{};

	Math::VectorTransform(hitbox->bbmin, m.matrix[hitbox->bone], min);
	Math::VectorTransform(hitbox->bbmax, m.matrix[hitbox->bone], max);

	return (min + max) / 2.0f;
}

Vector C_BasePlayer::get_hitbox_pos(int hitbox_id)
{
		auto studio_model = g_MdlInfo->GetStudiomodel(GetModel());
		if (studio_model) {
			auto hitbox = studio_model->GetHitboxSet(0).value()->GetHitbox(hitbox_id).value();
			if (hitbox) {
				auto
					min = Vector{},
					max = Vector{};

				Math::VectorTransform(hitbox->bbmin, player_matrixes[this->EntIndex()].matrix[hitbox->bone], min);
				Math::VectorTransform(hitbox->bbmax, player_matrixes[this->EntIndex()].matrix[hitbox->bone], max);

				return (min + max) / 2.0f;
			}
		}
	return Vector{};
}

bool C_BasePlayer::get_hitbox_pos(int hitbox, Vector &output)
{
	if (hitbox >= HITBOX_MAX)
		return false;

	const model_t *model = this->GetModel();
	if (!model)
		return false;

	studiohdr_t *studioHdr = g_MdlInfo->GetStudiomodel(model);
	if (!studioHdr)
		return false;

	mstudiobbox_t *studioBox = studioHdr->GetHitboxSet(0).value()->GetHitbox(hitbox).value();
	if (!studioBox)
		return false;

	Vector min, max;
	Math::VectorTransform(studioBox->bbmin, player_matrixes[this->EntIndex()].matrix[studioBox->bone], min);
	Math::VectorTransform(studioBox->bbmax, player_matrixes[this->EntIndex()].matrix[studioBox->bone], max);

	output = (min + max) * 0.5f;

	return true;
}

Vector C_BasePlayer::get_bone_pos(int bone)
{
		return player_matrixes[this->EntIndex()].matrix[bone].at(3);
}

Vector C_BasePlayer::get_backtracked_bone_pos(int bone, matrixes m)
{
	return m.matrix[bone].at(3);
}

bool C_BasePlayer::is_visible()
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = g_LocalPlayer;

	auto bone_matrix = player_matrixes[this->EntIndex()].matrix;

	matrix3x4_t hitbox = bone_matrix[Hitboxes::HITBOX_HEAD];
	//-------------------------

	ray.Init(g_LocalPlayer->get_eye_pos(), Vector(hitbox[0][3], hitbox[1][3], hitbox[2][3]));
	g_EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	return tr.hit_entity == this || tr.fraction > 0.97f;
}

bool C_BasePlayer::is_bone_visible(int i)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = g_LocalPlayer;

	auto bone_matrix = player_matrixes[this->EntIndex()].matrix;

	matrix3x4_t hitbox = bone_matrix[i];
	//-------------------------

	ray.Init(g_LocalPlayer->get_eye_pos(), Vector(hitbox[0][3], hitbox[1][3], hitbox[2][3]));
	g_EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	return tr.hit_entity == this || tr.fraction > 0.97f;
}

bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, int hitbox)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	auto endpos = player->get_hitbox_pos(hitbox);

	ray.Init(get_eye_pos(), endpos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, const Vector& pos)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	//auto start = get_eye_pos();
	//auto dir = (pos - start).Normalized();

	ray.Init(get_eye_pos(), pos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

void C_BasePlayer::UpdateClientSideAnimation()
{
	return call_vfunc<void(__thiscall*)(void*)>(this, 224)(this);
}

void C_BasePlayer::set_abs_angle(QAngle wantedang)
{
	typedef void(__thiscall* SetAngleFn)(void*, const QAngle &);
	static SetAngleFn set_abs_angle = reinterpret_cast<SetAngleFn>(Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1").value());
	set_abs_angle(this, wantedang);
}

void C_BasePlayer::set_abs_origin(Vector & origin)
{
		using SetAbsOriginFn = void(__thiscall*)(void*, const Vector &);
		static SetAbsOriginFn SetAbsOrigin = (SetAbsOriginFn)(Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8").value());
		SetAbsOrigin(this, origin);
}

Vector C_BasePlayer::get_abs_origin()
{
		__asm {
			MOV ECX, this
			MOV EAX, DWORD PTR DS : [ECX]
			CALL DWORD PTR DS : [EAX + 0x28]
		}
}

bool C_BasePlayer::is_vector_visible(Vector vec)
{
		trace_t tr;
		Ray_t ray;
		CTraceFilter filter;
		filter.pSkip = g_LocalPlayer;

		ray.Init(g_LocalPlayer->get_eye_pos(), vec);
		g_EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

		return tr.hit_entity == this;
}

void C_BasePlayer::InvalidateBoneCache()
{
	static auto InvalidateBoneCacheFn = Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), "80 3D ?? ?? ?? ?? ?? 74 16 A1 ?? ?? ?? ?? 48 C7 81").value();
	reinterpret_cast<void(__fastcall*)(void*)>(InvalidateBoneCacheFn)(this);
}

QAngle* C_BasePlayer::GetVAngles()
{
	static auto deadflag = NetvarSys::Get().GetOffset("DT_BasePlayer", "deadflag");
	return (QAngle*)((uintptr_t)this + deadflag + 0x4);
}

void C_BasePlayer::SetVAngles(QAngle ang)
{
	static auto deadflag = NetvarSys::Get().GetOffset("DT_BasePlayer", "deadflag");
	*(QAngle*)((uintptr_t)this + deadflag + 0x4) = ang;
}

void C_BaseAttributableItem::SetGloveModelIndex(int modelIndex)
{
	return call_vfunc<void(__thiscall*)(void*, int)>(this, 75)(this, modelIndex);
}

float C_BasePlayer::GetFlashBangTime()
{

	static uint32_t m_flFlashBangTime = *(uint32_t*)((uint32_t)Utils::PatternScan(GetModuleHandleA("client_panorama.dll"),
		"F3 0F 10 86 ?? ?? ?? ?? 0F 2F 40 10 76 30").value() + 4);
	return *(float*)(this + m_flFlashBangTime);
	//return *(float*)((uintptr_t)this + 0xa308);
}

void C_BaseViewModel::SendViewModelMatchingSequence(int sequence)
{
	return call_vfunc<void(__thiscall*)(void*, int)>(this, 244)(this, sequence);
}

CUtlVector<IRefCounted*>& C_EconItemView::m_CustomMaterials()
{
	return *(CUtlVector<IRefCounted*>*)((uintptr_t)this + 0x14);
}

CUtlVector<IRefCounted*>& C_EconItemView::m_VisualsDataProcessors()
{
	static auto inReload = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "81 C7 ? ? ? ? 8B 4F 0C 8B 57 04 89 4C 24 0C").value() + 2);
	return *(CUtlVector<IRefCounted*>*)((uintptr_t)this + inReload);
}

float_t C_BasePlayer::m_flSpawnTime()
{
	return *(float_t*)((uintptr_t)this + 0xA360);
}

bool C_BasePlayer::IsNotTarget()
{
	return !this || !IsPlayer() || this == g_LocalPlayer || m_iHealth() <= 0 || is_dormant() || !is_alive() || m_bGunGameImmunity() || (m_fFlags() & FL_FROZEN) || GetClientClass()->m_ClassID != 35;
}