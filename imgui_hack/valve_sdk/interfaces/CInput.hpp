#pragma once

#include "../Misc/CUserCmd.hpp"
#include "../Misc/vfunc.hpp"
#define MULTIPLAYER_BACKUP 150

class CInput
{
public:
	char pad_0000[172]; //0x0000
	bool N0000004E; //0x00AC
	bool m_fCameraInThirdPerson; //0x00AD
	char pad_00AE[2]; //0x00AE
	QAngle m_vecCameraOffset; //0x00B0

	inline CUserCmd* GetUserCmd(int sequence_number);
	inline CUserCmd * GetUserCmd(int nSlot, int sequence_number);
	inline CVerifiedUserCmd* GetVerifiedCmd(int sequence_number);
};

CUserCmd* CInput::GetUserCmd(int sequence_number)
{
	using OriginalFn = CUserCmd * (__thiscall *)(void *, int, int);
	return call_vfunc<OriginalFn>(this, 8)(this, 0, sequence_number);
}

CUserCmd *CInput::GetUserCmd(int nSlot, int sequence_number)
{
	typedef CUserCmd*(__thiscall *GetUserCmd_t)(void*, int, int);
	return call_vfunc<GetUserCmd_t>(this, 8)(this, nSlot, sequence_number);
}

CVerifiedUserCmd* CInput::GetVerifiedCmd(int sequence_number)
{
	auto verifiedCommands = *(CVerifiedUserCmd **)(reinterpret_cast<uint32_t>(this) + 0xF8);
	return &verifiedCommands[sequence_number % MULTIPLAYER_BACKUP];
}