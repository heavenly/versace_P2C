#pragma once 

#include "IClientNetworkable.hpp"
#include "IClientRenderable.hpp"
#include "IClientUnknown.hpp"
#include "IClientThinkable.hpp"

struct SpatializationInfo_t;

class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	virtual void Release(void) = 0;
};

#pragma pack(push, 1)
class CCSWeapData
{
public:
	std::uint8_t pad_0x0000[0x4]; //0x0000
	char* weapon_name; //0x0004 
	std::uint8_t pad_0x0008[0xC]; //0x0008
	std::uint32_t max_clip; //0x0014 
	std::uint8_t pad_0x0018[0x68]; //0x0018
	char* ammo_name; //0x0080 
	char* ammo_name_2; //0x0084 
	char* hud_name; //0x0088 
	char* weapon_id; //0x008C 
	std::uint8_t pad_0x0090[0x3C]; //0x0090
	std::uint32_t type; //0x00CC 
	std::uint32_t price; //0x00D0 
	std::uint32_t reward; //0x00D4 
	std::uint8_t pad_0x00D8[0x14]; //0x00D8
	std::uint8_t full_auto; //0x00EC 
	std::uint8_t pad_0x00ED[0x3]; //0x00ED
	std::uint32_t damage; //0x00F0 u un
	float armor_ratio; //0x00F4 
	std::uint32_t bullets; //0x00F8 
	float penetration; //0x00FC 
	std::uint8_t pad_0x0100[0x8]; //0x0100
	float range; //0x0108 
	float range_modifier; //0x010C 
	std::uint8_t pad_0x0110[0x20]; //0x0110
	float max_speed; //0x0130 
	float max_speed_alt; //0x0134 
	std::uint8_t pad_0x0138[0x108]; //0x0138
};
#pragma pack(pop)

class IWeaponSystem
{
	virtual void unused0() = 0;
	virtual void unused1() = 0;
};