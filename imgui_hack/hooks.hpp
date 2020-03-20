#pragma once

#include "valve_sdk/csgostructs.hpp"
#include "helpers/vfunc_hook.hpp"
#include <d3d9.h>

namespace index
{
	constexpr auto EmitSound1 = 5;
	constexpr auto EmitSound2 = 6;
    constexpr auto EndScene = 42;
	constexpr auto SceneEnd = 9;
    constexpr auto Reset = 16;
    constexpr auto PaintTraverse = 41;
    constexpr auto CreateMove = 22;
    constexpr auto PlaySound = 82;
    constexpr auto FrameStageNotify = 37;
    constexpr auto DrawModelExecute = 21;
    constexpr auto DoPostScreenSpaceEffects = 44;
	constexpr auto SvCheatsGetBool = 13;
	constexpr auto OverrideView = 18;
	constexpr auto LockCursor = 67;
	constexpr auto DispatchUserMsg = 38;
	constexpr auto ShouldDrawViewModel = 27;
	constexpr auto FindMDL = 10;
	constexpr auto GetPlayerInfo = 8;
	constexpr auto Present = 0;
	constexpr auto GetFriendCount = 3;
	constexpr auto GetFriendByIndex = 4;
	constexpr auto GetFriendPersonaState = 6;
}

namespace Hooks
{
    void Initialize();
    void Shutdown();

    extern vfunc_hook hlclient_hook;
    extern vfunc_hook direct3d_hook;
    extern vfunc_hook vguipanel_hook;
    extern vfunc_hook vguisurf_hook;
    extern vfunc_hook mdlrender_hook;
    extern vfunc_hook viewrender_hook;
	extern vfunc_hook renderview_hook;
	extern vfunc_hook findmdl_hook;

    using EndScene            = long(__stdcall *)(IDirect3DDevice9*);
    using Reset               = long(__stdcall *)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
    using CreateMove          = void(__thiscall*)(IBaseClientDLL*, int, float, bool);
    using PaintTraverse       = void(__thiscall*)(IPanel*, vgui::VPANEL, bool, bool);
	using EmitSound1          = void(__thiscall*)(void*, IRecipientFilter&, int, int, const char*, unsigned int, const char*, float, int, float, int, int, const Vector*, const Vector*, void*, bool, float, int, int);
    using FrameStageNotify    = void(__thiscall*)(IBaseClientDLL*, ClientFrameStage_t);
    using PlaySound           = void(__thiscall*)(ISurface*, const char* name);
	using LockCursor_t        = void(__thiscall*)(ISurface*);
    using DrawModelExecute    = void(__thiscall*)(IVModelRender*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
    using FireEvent           = bool(__thiscall*)(IGameEventManager2*, IGameEvent* pEvent);
    using DoPostScreenEffects = int(__thiscall*)(IClientMode*, int);
	using OverrideView = void(__thiscall*)(IClientMode*, CViewSetup*);
	using SceneEnd = void(__fastcall*)(void*, void*);
	using DispatchUserMessage_t = bool(__thiscall*)(void*, int, unsigned int, unsigned int, bf_read*);
	using ShouldDrawViewmodel = bool(__stdcall*)(void*);
	using FindMDL = MDLHandle_t(__thiscall*)(void*, char*);
	using GetPlayerInfo_t = bool(__thiscall*)(void*, int, player_info_t*);
	using PresentFn = long(__stdcall*)(IDirect3DDevice9* pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);

    long __stdcall hkEndScene(IDirect3DDevice9* device);
    long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
    void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket);
    void __stdcall hkCreateMove_Proxy(int sequence_number, float input_sample_frametime, bool active);
    void __stdcall hkPaintTraverse(vgui::VPANEL panel, bool forceRepaint, bool allowForce);
    void __stdcall hkDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld);
    void __stdcall hkFrameStageNotify(ClientFrameStage_t stage);
	void __stdcall hkLockCursor();
    int  __stdcall hkDoPostScreenEffects(int a1);
	void __fastcall hkSceneEnd(void* thisptr, void* edx);
	bool __fastcall DispatchUserMessage_h(void *thisptr, void*, unsigned int msg_type, unsigned int unk1, unsigned int nBytes, bf_read *msg_data);
	bool __stdcall hkDrawViewModel();
	MDLHandle_t   __fastcall  hk_findmdl(void* ecx, void* edx, char* file_path);
	bool __fastcall hkGetPlayerInfo(void* ecx, void* edx, int ent_num, player_info_t *pinfo);
	long __stdcall hkPresent(IDirect3DDevice9* device, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion);
	int __fastcall hk_GetFriendCount(void* ecx, void* edx, int iFriendFlags);
	void __fastcall hk_GetFriendByIndex(void* ecx, void* edx, std::uint64_t* retn, int iFriend, int iFriendFlags);
	EPersonaState __fastcall hk_GetFriendPersonaState(void* ecx, void* edx, CSteamID steamIDFriend);
	void __fastcall hkOverrideView(void* _this, void* _edx, CViewSetup* setup);
}
