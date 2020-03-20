#pragma once

#define NOMINMAX
#include <Windows.h>

#include "Interfaces/IBaseClientDll.hpp"
#include "Interfaces/IClientEntityList.hpp"
#include "Interfaces/IClientMode.hpp"
#include "Interfaces/IConVar.hpp"
#include "Interfaces/ICvar.hpp"
#include "Interfaces/IEngineTrace.hpp"
#include "Interfaces/IVEngineClient.hpp"
#include "Interfaces/IVDebugOverlay.hpp"
#include "Interfaces/ISurface.hpp"
#include "Interfaces/CInput.hpp"
#include "Interfaces/IVModelInfoClient.hpp"
#include "Interfaces/IVModelRender.hpp"
#include "Interfaces/IRenderView.hpp"
#include "Interfaces/IGameEventmanager.hpp"
#include "Interfaces/IMaterialSystem.hpp"
#include "Interfaces/IMoveHelper.hpp"
#include "Interfaces/IMDLCache.hpp"
#include "Interfaces/IPrediction.hpp"
#include "Interfaces/IPanel.hpp"
#include "Interfaces/IEngineSound.hpp"
#include "Interfaces/IViewRender.hpp"
#include "Interfaces/CClientState.hpp"
#include "Interfaces/IPhysics.hpp"
#include "Interfaces/ILocalize.hpp"
#include "../IViewRenderBeams.h"
#include "../ISteamFriends.h"
#include "../ISteamClient.h"
#include "../ISteamUser.h"

#include "Misc/glow_outline_effect.hpp"

struct IDirect3DDevice9;

namespace Interfaces
{
    void Initialize();
    void Dump();
}

extern IVEngineClient*       g_EngineClient;
extern IBaseClientDLL*       g_CHLClient;
extern IClientEntityList*    g_EntityList;
extern CGlobalVarsBase*      g_GlobalVars;
extern IEngineTrace*         g_EngineTrace;
extern ICvar*                g_CVar;
extern IPanel*               g_VGuiPanel;
extern IClientMode*          g_ClientMode;
extern IVDebugOverlay*       g_DebugOverlay;
extern ISurface*             g_VGuiSurface;
extern CInput*               g_Input;
extern IVModelInfoClient*    g_MdlInfo;
extern IVModelRender*        g_MdlRender;
extern IVRenderView*         g_RenderView;
extern IMaterialSystem*      g_MatSystem;
extern IGameEventManager2*   g_GameEvents;
extern IViewRenderBeams* g_RenderBeams;
extern IMoveHelper*          g_MoveHelper;
extern IMDLCache*            g_MdlCache;
extern IPrediction*          g_Prediction;
extern CGameMovement*        g_GameMovement;
extern IEngineSound*         g_EngineSound;
extern CGlowObjectManager*   g_GlowObjManager;
extern IViewRender*          g_ViewRender;
extern IDirect3DDevice9*     g_D3DDevice9;
extern CClientState*         g_ClientState;
extern IPhysicsSurfaceProps* g_PhysSurface;
extern ILocalize* g_Localize;
extern ISteamClient* g_SteamClient;
extern ISteamFriends* g_SteamFriends;
extern ISteamUser* g_SteamUser;

template<typename... Args>
void ConMsg(const char* pMsg, Args... args)
{
    static auto import = (void(*)(const char*, ...))GetProcAddress(GetModuleHandleW(L"tier0.dll"), "?ConMsg@@YAXPBDZZ");
    return import(pMsg, args...);
}
template<typename... Args>
void ConColorMsg(const Color& clr, const char* pMsg, Args... args)
{
    static auto import = (void(*)(const Color&, const char*, ...))GetProcAddress(GetModuleHandleW(L"tier0.dll"), "?ConColorMsg@@YAXABVColor@@PBDZZ");
    return import(clr, pMsg, args...);
}

#include "Misc/EHandle.hpp"

class C_LocalPlayer
{
    friend bool operator==(const C_LocalPlayer& lhs, void* rhs);
public:
    C_LocalPlayer() : m_local(nullptr) {}

    operator bool() const { return *m_local != nullptr; }
    operator C_BasePlayer*() const { return *m_local; }

    C_BasePlayer* operator->() { return *m_local; }

private:
    C_BasePlayer** m_local;
};

extern C_LocalPlayer g_LocalPlayer;

