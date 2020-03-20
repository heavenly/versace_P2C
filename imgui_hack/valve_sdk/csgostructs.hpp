#pragma once

#include "sdk.hpp"
#include <array>
#include "../IRefCounted.h"
#include "../helpers/utils.hpp"
#include "../player_matrix.h"
#include "netvars.hpp"
#include "misc/vfunc.hpp"

#define NETVAR(type, name, table, netvar)                           \
	type& name##() const {                                          \
		static int _##name = NetvarSys::Get().GetOffset(table, netvar);     \
		return *(type*)((uintptr_t)this + _##name);                 \
    }


#define NETVARADDOFFS(type, name, table, netvar, offs)                           \
    type& name##() const {                                          \
        static int _##name = NetvarSys::Get().GetOffset(table, netvar) + offs;     \
        return *(type*)((uintptr_t)this + _##name);                 \
	}

#define PNETVAR(type, name, table, netvar)                           \
    type* name##() const {                                          \
        static int _##name = NetvarSys::Get().GetOffset(table, netvar);     \
        return (type*)((uintptr_t)this + _##name);                 \
    }

#define NETPROP(name, table, netvar) static RecvProp* name() \
{ \
	static auto prop_ptr = NetvarSys::Get().GetNetvarProp(table,netvar); \
	return prop_ptr; \
}

struct datamap_t;
class AnimationLayer;
class c_baseplayer_animstate;
class ccs_player_animstate;
class CCSGOPlayerAnimState;
class C_BaseEntity;

enum CSWeaponType
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_PLACEHOLDER,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_UNKNOWN
};

class C_BaseEntity;

// Created with ReClass.NET by KN4CK3R
class CHudTexture
{
public:
	char szShortName[64];    //0x0000
	char szTextureFile[64];  //0x0040
	bool bRenderUsingFont;   //0x0080
	bool bPrecached;         //0x0081
	int8_t cCharacterInFont; //0x0082
	uint8_t pad_0083[1];     //0x0083
	uint32_t hFont;          //0x0084
	int32_t iTextureId;      //0x0088
	float afTexCoords[4];    //0x008C
	uint8_t pad_009C[16];    //0x009C
};

class C_EconItemView
{
private:
	using str_32 = char[32];
public:
	NETVAR(int32_t, m_bInitialized, "DT_BaseAttributableItem", "m_bInitialized");
	NETVAR(short, m_iItemDefinitionIndex, "DT_BaseAttributableItem", "m_iItemDefinitionIndex");
	NETVAR(int32_t, m_iEntityLevel, "DT_BaseAttributableItem", "m_iEntityLevel");
	NETVAR(int32_t, m_iAccountID, "DT_BaseAttributableItem", "m_iAccountID");
	NETVAR(int32_t, m_iItemIDLow, "DT_BaseAttributableItem", "m_iItemIDLow");
	NETVAR(int32_t, m_iItemIDHigh, "DT_BaseAttributableItem", "m_iItemIDHigh");
	NETVAR(int32_t, m_iEntityQuality, "DT_BaseAttributableItem", "m_iEntityQuality");
	NETVAR(str_32, m_iCustomName, "DT_BaseAttributableItem", "m_szCustomName");

	CUtlVector<IRefCounted*>& m_CustomMaterials();
	CUtlVector<IRefCounted*>& m_VisualsDataProcessors();
};

class C_BaseEntity : public IClientEntity
{
public:
	datamap_t * GetDataDescMap() {
		typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
		return call_vfunc<o_GetPredDescMap>(this, 15)(this);
	}

	datamap_t *GetPredDescMap() {
		typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
		return call_vfunc<o_GetPredDescMap>(this, 17)(this);
	}
	static __forceinline C_BaseEntity* GetEntityByIndex(int index) {
		return static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(index));
	}
	static __forceinline C_BaseEntity* get_entity_from_handle(CBaseHandle h) {
		return static_cast<C_BaseEntity*>(g_EntityList->GetClientEntityFromHandle(h));
	}

	NETVAR(int32_t, m_nModelIndex, "DT_BaseEntity", "m_nModelIndex");
	NETVAR(int32_t, m_iTeamNum, "DT_BaseEntity", "m_iTeamNum");
	NETVAR(Vector, m_vecOrigin, "DT_BaseEntity", "m_vecOrigin");
	NETVAR(Vector, m_vecAngles, "DT_BaseEntity", "m_vecAngles");
	NETVAR(bool, m_bShouldGlow, "DT_DynamicProp", "m_bShouldGlow");
	NETVAR(float, GetSimulationTime, "DT_BaseEntity", "m_flSimulationTime");
	NETVAR(CHandle<C_BasePlayer>, m_hOwnerEntity, "DT_BaseEntity", "m_hOwnerEntity");
	NETVAR(bool, m_bSpotted, "DT_BaseEntity", "m_bSpotted");
	NETVAR(float_t, m_flC4Blow, "DT_PlantedC4", "m_flC4Blow");
	NETVAR(float_t, m_hBombDefuser, "DT_PlantedC4", "m_hBombDefuser");
	NETVAR(float_t, m_flDefuseCountDown, "DT_PlantedC4", "m_flDefuseCountDown");

	NETVAR(Vector, get_min, "DT_BaseEntity", "m_Collision", "m_vecMins");
	NETVAR(Vector, get_max, "DT_BaseEntity", "m_Collision", "m_vecMaxs");

	NETVAR(Vector, skybox3d_origin, "DT_BasePlayer", "DT_LocalPlayerExclusive", "DT_Local", "m_skybox3d.origin");
	NETVAR(int, skybox3d_scale, "DT_BasePlayer", "DT_LocalPlayerExclusive", "DT_Local", "m_skybox3d.scale");

	const matrix3x4_t& m_rgflCoordinateFrame()
	{
		static auto _m_rgflCoordinateFrame = NetvarSys::Get().GetOffset("DT_CSPlayer", "m_CollisionGroup") - 0x30;
		return *(matrix3x4_t*)((uintptr_t)this + _m_rgflCoordinateFrame);
	}

	int &get_take_damage() {
		return *reinterpret_cast<int *>(uintptr_t(this) + 0x280);
	}

	Vector world_space_center() 
	{
		Vector Max = this->get_max() + this->m_vecOrigin();
		Vector Min = this->get_min() + this->m_vecOrigin();
		Vector Size = Max - Min;
		Size /= 2;
		Size += Min;
		return Size;
	}

	bool IsPlayer();
	bool IsWeapon();
	bool IsPlantedC4();
	bool is_defuse_kit();
	bool IsLoot();
	//bool isSpotted();
};

class C_PlantedC4
{
public:
	NETVAR(bool, m_bBombTicking, "DT_PlantedC4", "m_bBombTicking");
	NETVAR(bool, m_bBombDefused, "DT_PlantedC4", "m_bBombDefused");
	NETVAR(float, m_flC4Blow, "DT_PlantedC4", "m_flC4Blow");
	NETVAR(float, m_flTimerLength, "DT_PlantedC4", "m_flTimerLength");
	NETVAR(float, m_flDefuseLength, "DT_PlantedC4", "m_flDefuseLength");
	NETVAR(float, m_flDefuseCountDown, "DT_PlantedC4", "m_flDefuseCountDown");
	NETVAR(CHandle<C_BasePlayer>, m_hBombDefuser, "DT_PlantedC4", "m_hBombDefuser");
};

class C_BaseAttributableItem : public C_BaseEntity
{
public:
	NETVAR(uint64_t, m_OriginalOwnerXuid, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	NETVAR(int32_t, m_OriginalOwnerXuidLow, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	NETVAR(int32_t, m_OriginalOwnerXuidHigh, "DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh");
	NETVAR(int32_t, m_iItemIDHigh, "DT_BaseAttributableItem", "m_iItemIDHigh");
	NETVAR(int32_t, GetAccountID, "DT_BaseAttributableItem", "m_iAccountID");
	NETVAR(int32_t, m_nFallbackStatTrak, "DT_BaseAttributableItem", "m_nFallbackStatTrak");
	NETVAR(int32_t, m_nFallbackPaintKit, "DT_BaseAttributableItem", "m_nFallbackPaintKit");
	NETVAR(int32_t, m_nFallbackSeed, "DT_BaseAttributableItem", "m_nFallbackSeed");
	NETVAR(float_t, m_flFallbackWear, "DT_BaseAttributableItem", "m_flFallbackWear");
	NETVAR(short, m_iItemDefinitionIndex, "DT_BaseAttributableItem", "m_iItemDefinitionIndex");
	NETVAR(C_EconItemView, m_Item2, "DT_BaseAttributableItem", "m_Item");

	C_EconItemView& m_Item()
	{
		// Cheating. It should be this + m_Item netvar but then the netvars inside C_EconItemView wont work properly.
		// A real fix for this requires a rewrite of the netvar manager
		return *(C_EconItemView*)this;
	}
	void SetGloveModelIndex(int modelIndex);
	void SetModelIndex(const int index)
	{
		return call_vfunc<void(__thiscall*)(C_BaseEntity*, int)>(this, 75)(this, index);
	}
};

class C_BaseWeaponWorldModel : public C_BaseEntity
{
public:
	NETVAR(int32_t, m_nModelIndex, "DT_BaseWeaponWorldModel", "m_nModelIndex");
};

class C_AttributeManager
{
public:
	char pad_0000[64]; //0x0000
	C_EconItemView m_item; //0x0040
};

class C_WeaponCSBase : public IClientEntity
{
public:
	char	_pad_0x0000[2508];
	CUtlVector<IRefCounted*> m_CustomMaterials; //0x09DC
	char	_pad_0x09F0[9120];
	C_AttributeManager m_AttributeManager; //0x2D80
	char	_pad_0x2D94[812];
	bool m_bCustomMaterialInitialized; //0x3330
};

class C_BaseCombatWeapon : public C_BaseAttributableItem
{
public:
	NETVAR(float_t, m_flNextPrimaryAttack, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
	NETVAR(float_t, m_flNextSecondaryAttack, "DT_BaseCombatWeapon", "m_flNextSecondaryAttack");
	NETVAR(int32_t, m_iClip1, "DT_BaseCombatWeapon", "m_iClip1");
	NETVAR(int32_t, m_iClip2, "DT_BaseCombatWeapon", "m_iPrimaryReserveAmmoCount");
	NETVAR(float_t, m_flRecoilIndex, "DT_WeaponCSBase", "m_flRecoilIndex");
	NETVAR(int32_t, m_iViewModelIndex, "DT_BaseCombatWeapon", "m_iViewModelIndex");
	NETVAR(int32_t, m_iWorldModelIndex, "DT_BaseCombatWeapon", "m_iWorldModelIndex");
	NETVAR(int32_t, m_iWorldDroppedModelIndex, "DT_BaseCombatWeapon", "m_iWorldDroppedModelIndex");
	NETVAR(bool, m_bPinPulled, "DT_BaseCSGrenade", "m_bPinPulled");
	NETVAR(float_t, m_fThrowTime, "DT_BaseCSGrenade", "m_fThrowTime");
	NETVAR(float_t, m_flPostponeFireReadyTime, "DT_BaseCombatWeapon", "m_flPostponeFireReadyTime");
	NETVAR(float_t, get_accuracy, "DT_WeaponCSBase", "m_fAccuracyPenalty");
	NETVAR(CHandle<C_BaseWeaponWorldModel>, m_hWeaponWorldModel, "DT_BaseCombatWeapon", "m_hWeaponWorldModel");


	CCSWeapData* get_cs_wpn_data();
	bool has_bullets();
	bool can_fire();
	bool is_grenade();
	bool is_knife();
	bool is_taser();
	bool is_reloading();
	bool is_rifle();
	bool is_pistol();
	bool is_sniper();
	bool is_gun();
	CUtlVector<IRefCounted*>& m_CustomMaterials();
	bool* m_bCustomMaterialInitialized();
	float get_spread();
	void update_accuracy();
};

class c_ragdoll : public C_BaseEntity {
public:
	NETVAR(Vector, m_vecForce, "DT_CSRagdoll", "m_vecForce");
	NETVAR(Vector, m_vecRagdollVelocity, "DT_CSRagdoll", "m_vecRagdollVelocity");
};

class C_BasePlayer : public C_BaseEntity
{
public:
	static __forceinline C_BasePlayer* GetPlayerByUserId(int id)
	{
		return static_cast<C_BasePlayer*>(GetEntityByIndex(g_EngineClient->GetPlayerForUserID(id)));
	}

	static __forceinline C_BasePlayer* GetPlayerByIndex(int i)
	{
		return static_cast<C_BasePlayer*>(GetEntityByIndex(i));
	}

	NETVAR(bool, m_bHasDefuser, "DT_CSPlayer", "m_bHasDefuser");
	NETVAR(bool, m_bGunGameImmunity, "DT_CSPlayer", "m_bGunGameImmunity");
	NETVAR(int32_t, m_iShotsFired, "DT_CSPlayer", "m_iShotsFired");
	NETVAR(QAngle, m_angEyeAngles, "DT_CSPlayer", "m_angEyeAngles[0]");
	NETVAR(int, m_ArmorValue, "DT_CSPlayer", "m_ArmorValue");
	NETVAR(bool, m_bHasHeavyArmor, "DT_CSPlayer", "m_bHasHeavyArmor");
	NETVAR(bool, m_bHasHelmet, "DT_CSPlayer", "m_bHasHelmet");
	NETVAR(bool, m_bIsScoped, "DT_CSPlayer", "m_bIsScoped");
	NETVAR(float, m_flLowerBodyYawTarget, "DT_CSPlayer", "m_flLowerBodyYawTarget");
	NETVAR(int32_t, m_iHealth, "DT_BasePlayer", "m_iHealth");
	NETVAR(int32_t, m_lifeState, "DT_BasePlayer", "m_lifeState");

	//broken for some reason
	//NETVAR(int32_t, m_MoveType, "DT_BasePlayer", "m_MoveType");

	int32_t m_MoveType()
	{
		static unsigned int _m_MoveType = Utils::FindInDataMap(GetPredDescMap(), "m_MoveType");
		return *(int32_t*)((uintptr_t)this + _m_MoveType);
	}

	NETVAR(int32_t, m_fFlags, "DT_BasePlayer", "m_fFlags");
	NETVAR(int32_t, m_nTickBase, "DT_BasePlayer", "m_nTickBase");
	NETVAR(Vector, m_vecViewOffset, "DT_BasePlayer", "m_vecViewOffset[0]");
	NETVAR(QAngle, m_viewPunchAngle, "DT_BasePlayer", "m_viewPunchAngle");
	NETVAR(QAngle, m_aimPunchAngle, "DT_BasePlayer", "m_aimPunchAngle");
	NETVAR(CHandle<C_BaseViewModel>, m_hViewModel, "DT_BasePlayer", "m_hViewModel[0]");
	NETVAR(Vector, m_vecVelocity, "DT_BasePlayer", "m_vecVelocity[0]");
	NETVAR(float, m_flMaxspeed, "DT_BasePlayer", "m_flMaxspeed");
	NETVAR(CHandle<C_BasePlayer>, m_hObserverTarget, "DT_BasePlayer", "m_hObserverTarget");
	NETVAR(float, m_flFlashMaxAlpha, "DT_CSPlayer", "m_flFlashMaxAlpha");
	NETVAR(int32_t, m_nHitboxSet, "DT_BaseAnimating", "m_nHitboxSet");
	NETVAR(bool, m_bClientSideAnimation, "DT_BasePlayer", "m_bClientSideAnimation");

	NETVAR(CHandle<C_BaseCombatWeapon>, m_hActiveWeapon, "DT_BaseCombatCharacter", "m_hActiveWeapon");
	NETVAR(int32_t, m_iAccount, "DT_CSPlayer", "m_iAccount");
	NETVAR(float, m_flFlashDuration, "DT_CSPlayer", "m_flFlashDuration");
	NETVAR(float, m_flSimulationTime, "DT_BaseEntity", "m_flSimulationTime");
	NETVARADDOFFS(float_t, m_flOldSimulationTime, "DT_BaseEntity", "m_flSimulationTime", 0x4);

	NETVAR(float, m_flCycle, "DT_ServerAnimationData", "m_flCycle");
	NETVAR(int, m_nSequence, "DT_BaseViewModel", "m_nSequence");
	PNETVAR(char, m_szLastPlaceName, "DT_BasePlayer", "m_szLastPlaceName");
	NETPROP(m_flLowerBodyYawTargetProp, "DT_CSPlayer", "m_flLowerBodyYawTarget");

	NETVAR(QAngle, m_angAbsAngles, "DT_BaseEntity", "m_angAbsAngles");
	NETVAR(Vector, m_angAbsOrigin, "DT_BaseEntity", "m_angAbsOrigin");
	NETVAR(float, m_flDuckSpeed, "DT_BaseEntity", "m_flDuckSpeed");
	NETVAR(float, m_flDuckAmount, "DT_BaseEntity", "m_flDuckAmount");
	std::array<float, 24> m_flPoseParameter() const {
		static int _m_flPoseParameter = NetvarSys::Get().GetOffset("DT_BaseAnimating", "m_flPoseParameter");
		return *(std::array<float, 24>*)((uintptr_t)this + _m_flPoseParameter);
	}

	PNETVAR(CHandle<C_BaseCombatWeapon>, m_hMyWeapons, "DT_BaseCombatCharacter", "m_hMyWeapons");
	PNETVAR(CHandle<C_BaseAttributableItem>, m_hMyWearables, "DT_BaseCombatCharacter", "m_hMyWearables");

	CUserCmd*& m_pCurrentCommand();

	void InvalidateBoneCache();
	int GetNumAnimOverlays();
	AnimationLayer *GetAnimOverlays();
	std::optional<AnimationLayer*> GetAnimOverlay(int i);
	int GetSequenceActivity(int sequence);
	c_baseplayer_animstate *get_base_player_animstate();
	ccs_player_animstate *get_player_animstate();

	static void UpdateAnimationState(c_baseplayer_animstate*state, QAngle angle);
	static void ResetAnimationState(c_baseplayer_animstate*state);
	void CreateAnimationState(c_baseplayer_animstate*state);

	float_t &m_surfaceFriction()
	{
		static unsigned int _m_surfaceFriction = Utils::FindInDataMap(GetPredDescMap(), "m_surfaceFriction");
		return *(float_t*)((uintptr_t)this + _m_surfaceFriction);
	}

	bool is_valid()
	{
		if (!this)
			return false;
		
		if (!this->is_alive())
			return false;

		if (this->is_dormant())
			return false;

		if (this->m_bGunGameImmunity())
			return false;

		return true;
	}

	Vector &m_vecBaseVelocity()
	{
		static unsigned int _m_vecBaseVelocity = Utils::FindInDataMap(GetPredDescMap(), "m_vecBaseVelocity");
		return *(Vector*)((uintptr_t)this + _m_vecBaseVelocity);
	}

	float_t &m_flMaxspeed()
	{
		static unsigned int _m_flMaxspeed = Utils::FindInDataMap(GetPredDescMap(), "m_flMaxspeed");
		return *(float_t*)((uintptr_t)this + _m_flMaxspeed);
	}

	float GetAnimationTime()
	{
		return *reinterpret_cast<float*>((DWORD)this + 0x3C);
	}

	Vector        get_eye_pos();
	player_info_t get_player_info();
	bool          is_alive();
	bool		  is_flashed();
	bool          HasC4();
	Vector get_backtracked_hitbox_pos(int hitbox_id, matrixes m);
	Vector        get_hitbox_pos(int hitbox_id);
	mstudiobbox_t * GetHitbox(int hitbox_id);
	bool          get_hitbox_pos(int hitbox, Vector &output);
	Vector        get_bone_pos(int bone);
	Vector        get_backtracked_bone_pos(int bone, matrixes m);
	bool is_visible();
	bool is_bone_visible(int i);
	bool          CanSeePlayer(C_BasePlayer* player, int hitbox);
	bool          CanSeePlayer(C_BasePlayer* player, const Vector& pos);
	void UpdateClientSideAnimation();
	void set_abs_angle(QAngle wantedang);
	void set_abs_origin(Vector &origin);
	Vector get_abs_origin();
	bool is_vector_visible(Vector vec);

	QAngle * GetVAngles();
	void SetVAngles(QAngle ang);

	float GetFlashBangTime();

	float_t m_flSpawnTime();
	bool IsNotTarget();

};

class C_BaseViewModel : public C_BaseEntity
{
public:
	NETVAR(int32_t, m_nModelIndex, "DT_BaseViewModel", "m_nModelIndex");
	NETVAR(int32_t, m_nViewModelIndex, "DT_BaseViewModel", "m_nViewModelIndex");
	NETVAR(CHandle<C_BaseCombatWeapon>, m_hWeapon, "DT_BaseViewModel", "m_hWeapon");
	NETVAR(CHandle<C_BasePlayer>, m_hOwner, "DT_BaseViewModel", "m_hOwner");
	NETPROP(m_nSequence, "DT_BaseViewModel", "m_nSequence");
	void SendViewModelMatchingSequence(int sequence);
};

class AnimationLayer
{
public:
	char  pad_0000[20];
	// These should also be present in the padding, don't see the use for it though
	//float	m_flLayerAnimtime;
	//float	m_flLayerFadeOuttime;
	uint32_t m_nOrder; //0x0014
	uint32_t m_nSequence; //0x0018
	float_t m_flPrevCycle; //0x001C
	float_t m_flWeight; //0x0020
	float_t m_flWeightDeltaRate; //0x0024
	float_t m_flPlaybackRate; //0x0028
	float_t m_flCycle; //0x002C
	void *m_pOwner; //0x0030 // player's thisptr
	char  pad_0038[4]; //0x0034
}; //Size: 0x0038

class c_baseplayer_animstate
{
public:
	char pad_0x0000[0x14]; //0x0000
	float m_flHowLongIsEntityWalking; //0x0014 counts up if full walk speed is achieved
	char pad_0x0018[0xC]; //0x0018
	float m_flIsWalking; //0x0024 Gets set up to 1.f if the entity is fully walking 
	float m_flTimeSinceWalkingWithMovementSpeedGreaterThenWalking; //0x0028 if speed is greater then walking it starts counting up
	float m_flTimeSinceMovementWasGreaterThenWalking; //0x002C Only resets if speed is greater then walking
	char pad_0x0030[0xC]; //0x0030
	float m_flTimeSinceStandingStill; //0x003C Not that agressive only counts up if speed is higher then 50.f +
	float m_flTimeSinceAllMovement; //0x0040 every slight movement will reset it
	char pad_0x0044[0x8]; //0x0044
	float m_flIsRunning; //0x004C Gets set up to 1.f if entity is fully running
	char pad_0x0050[0x14]; //0x0050
	void* pActiveWeapon; //0x0064 
	void* pLastActiveWeapon; //0x0068 
	float m_flLastClientSideAnimationUpdateTime; //0x006C 
	int m_iLastClientSideAnimationUpdateFramecount; //0x0070 
	float m_flEyePitch; //0x0074 
	float m_flEyeYaw; //0x0078 weird eyeyaw
	float m_flPitch; //0x007C 
	float m_flGoalFeetYaw; //0x0080 
	float m_flCurrentFeetYaw; //0x0084 
	float m_flCurrentTorsoYaw; //0x0088 torso yaw and unknown velocity lean have the same values for some reason
	float m_flUnknownVelocityLean; //0x008C 
	float m_flLeanAmmount; //0x0090 
	float m_flUnknown1; //0x0094 never gets set dunno
	float m_flFeetCycle; //0x0098 
	float m_flYawRate; //0x009C 
	float m_flUnknown2; //0x00A0 never gets set dont fucking know what that is
	float m_flDuckAmount; //0x00A4 this also shows the duck amount when youve landed
	float m_flLandingAmount; //0x00A8 the original name was retarded this fits more
	float m_flUnknown3; //0x00AC 
	Vector m_vOrigin; //0x00B0 
	Vector m_vLastOrigin; //0x00BC 
	float m_flVelocityX; //0x00C8 
	float m_flVelocityY; //0x00CC 
	float m_flUnknown4; //0x00D0 
	float m_flUnknownVelocityFloat1; //0x00D4 affected by movement & direction  
	float m_flUnknownVelocityFloat2; //0x00D8 affected by movement & direction
	float m_flUnknown5; //0x00DC 
	float m_flUnknownVelocityFloat3; //0x00E0 from -1 to 1 when moving and affected by direction
	float m_flUnknownVelocityFloat4; //0x00E4 from -1 to 1 when moving and affected by direction
	float m_flUnknown6; //0x00E8 
	float m_flCurrentVelocity; //0x00EC normal speed2d = entity moving speed/velocity 
	float m_flFallingAndJumpingVelocity; //0x00F0 falling down and jumping up velocity
	float m_flSpeedNormalized; //0x00F4  0 to 1 
	float m_flFeetSpeedForwardsOrSideWays; //0x00F8 from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
	float m_flFeetSpeedUnknownForwardOrSideways; //0x00FC from 0 to 3
	float m_flTimeSinceStartedMoving; //0x0100 valve something similar exists at 0x28 get some structure in here
	float m_flTimeSinceStoppedMoving; //0x0104 same thing as above 0x40 does nearly the same shit ffs
	unsigned char m_bOnGround; //0x0108 instead of making a bool valve decided to make it 1 byte and assign 0 - 1 and sometimes fucking 257 to it
	unsigned char m_bInHitGroundAnimation; //0x0109 dunno why they dont make simple bools but decide to make them 1 byte
	char pad_0x010A[0x6]; //0x010A
	float m_flInAirTime; //0x0110 could be neat to have
	float m_flLastOriginZ; //0x0114 
	float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x0118 pretty weird value ngl
	float m_flStopToFullRunningFraction; //0x011C from 0 to 1, doesnt change when walking or crouching, only running
	char pad_0x0120[0x4]; //0x0120
	float m_flUnknownFraction; //0x0124 affected while jumping and running, or when just jumping, 0 to 1
	char pad_0x0128[0xC]; //0x0128
	int m_bIsRunning; //0x0134 similar already exists at 0x4C ..
	char pad_0x0138[0x1F8]; //0x0138
	float m_flMinDesyncValue; //0x0330 I assume those are the values?
	float m_flMaxDesyncValue; //0x0334 I assume those are the values?
	float m_flMinYawUnknown; //0x0338 
	float m_flMaxYawUnknown; //0x033C 
	char pad_0x0340[0x100]; //0x0340
};

class ccs_player_animstate
{
public:

	virtual ~ccs_player_animstate() = 0;
	virtual void Update(float_t eyeYaw, float_t eyePitch) = 0;
};

class CCSGOPlayerAnimState
{
public:

	int32_t &m_iLastClientSideAnimationUpdateFramecount()
	{
		return *(int32_t*)((uintptr_t)this + 0x70);
	}

	float_t &m_flEyeYaw()
	{
		return *(float_t*)((uintptr_t)this + 0x78);
	}

	float_t &m_flEyePitch()
	{
		return *(float_t*)((uintptr_t)this + 0x7C);
	}

	float_t &m_flGoalFeetYaw()
	{
		return *(float_t*)((uintptr_t)this + 0x80);
	}

	float_t &m_flCurrentFeetYaw()
	{
		return *(float_t*)((uintptr_t)this + 0x84);
	}

	bool &m_bCurrentFeetYawInitialized()
	{
		return *(bool*)((uintptr_t)this + 0x88);
	}

	Vector &m_vecVelocity()
	{
		// Only on ground velocity
		return *(Vector*)((uintptr_t)this + 0xC8);
	}

	float &unknown_fraction()
	{
		return *(float*)((uintptr_t)this + 0x270);
	}

	float_t m_flVelocity()
	{
		return *(float_t*)((uintptr_t)this + 0xEC);
	}

	char pad_0x0000[0x344]; //0x0000
}; //Size=0x344

class DT_CSPlayerResource
{
public:
	PNETVAR(int32_t, m_nActiveCoinRank, "DT_CSPlayerResource", "m_nActiveCoinRank");
	PNETVAR(int32_t, m_nMusicID, "DT_CSPlayerResource", "m_nMusicID");
	PNETVAR(int32_t, m_nPersonaDataPublicLevel, "DT_CSPlayerResource", "m_nPersonaDataPublicLevel");
	PNETVAR(int32_t, m_nPersonaDataPublicCommendsLeader, "DT_CSPlayerResource", "m_nPersonaDataPublicCommendsLeader");
	PNETVAR(int32_t, m_nPersonaDataPublicCommendsTeacher, "DT_CSPlayerResource", "m_nPersonaDataPublicCommendsTeacher");
	PNETVAR(int32_t, m_nPersonaDataPublicCommendsFriendly, "DT_CSPlayerResource", "m_nPersonaDataPublicCommendsFriendly");
	PNETVAR(int32_t, m_iCompetitiveRanking, "DT_CSPlayerResource", "m_iCompetitiveRanking");
	PNETVAR(int32_t, m_iCompetitiveWins, "DT_CSPlayerResource", "m_iCompetitiveWins");
	PNETVAR(int32_t, m_iPlayerVIP, "DT_CSPlayerResource", "m_iPlayerVIP");
	PNETVAR(int32_t, m_iMVPs, "DT_CSPlayerResource", "m_iMVPs");
	PNETVAR(int32_t, m_iScore, "DT_CSPlayerResource", "m_iScore");
};