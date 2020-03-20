#include "sdk.hpp"
#include "../json.h"
#include "../Helpers/Utils.hpp"
#include <VMProtectSDK.h>
#include "../local_info.h"
#include "../aes.h"
#include "../http_client.h"

IVEngineClient* g_EngineClient = nullptr;
IBaseClientDLL* g_CHLClient = nullptr;
IClientEntityList* g_EntityList = nullptr;
CGlobalVarsBase* g_GlobalVars = nullptr;
IEngineTrace* g_EngineTrace = nullptr;
ICvar* g_CVar = nullptr;
IPanel* g_VGuiPanel = nullptr;
IClientMode* g_ClientMode = nullptr;
IVDebugOverlay* g_DebugOverlay = nullptr;
ISurface* g_VGuiSurface = nullptr;
CInput* g_Input = nullptr;
IVModelInfoClient* g_MdlInfo = nullptr;
IVModelRender* g_MdlRender = nullptr;
IVRenderView* g_RenderView = nullptr;
IMaterialSystem* g_MatSystem = nullptr;
IGameEventManager2* g_GameEvents = nullptr;
IViewRenderBeams* g_RenderBeams = nullptr;
IMoveHelper* g_MoveHelper = nullptr;
IMDLCache* g_MdlCache = nullptr;
IPrediction* g_Prediction = nullptr;
CGameMovement* g_GameMovement = nullptr;
IEngineSound* g_EngineSound = nullptr;
CGlowObjectManager* g_GlowObjManager = nullptr;
IViewRender* g_ViewRender = nullptr;
IDirect3DDevice9* g_D3DDevice9 = nullptr;
CClientState* g_ClientState = nullptr;
IPhysicsSurfaceProps* g_PhysSurface = nullptr;
ILocalize* g_Localize = nullptr;
ISteamClient* g_SteamClient = nullptr;
ISteamFriends* g_SteamFriends = nullptr;
ISteamUser* g_SteamUser = nullptr;
C_LocalPlayer         g_LocalPlayer;

const static std::string cheat_name = "VER$ACE";

namespace Interfaces
{
	CreateInterfaceFn get_module_factory(HMODULE module)
	{
		return reinterpret_cast<CreateInterfaceFn>(GetProcAddress(module, "CreateInterface"));
	}

	template<typename T>
	T* get_interface(CreateInterfaceFn f, const char* szInterfaceVersion)
	{
		auto result = reinterpret_cast<T*>(f(szInterfaceVersion, nullptr));

		if (!result) {
			throw std::runtime_error(std::string("[get_interface] Failed to GetOffset interface: ") + szInterfaceVersion);
		}

		return result;
	}

	void Initialize()
	{
#ifndef _DEBUG
		VMProtectBeginUltra("sdk.cpp");
#endif
		//autism code

		c_aes aes = c_aes();
		c_http_client http_client = c_http_client();
		std::string data = "username=" + aes.encrypt_str(info_lp::username) + "&hwid=" + aes.encrypt_str(info_lp::hwid);

		std::string response_string = http_client.send_post(L"authentication/get_sigs.php", data);

		if(response_string.length() == 0)
		{
			MessageBoxA(NULL, "failed sigs check", "failed.", 0);
			return;
		}
		
		std::stringstream ss;
		ss.str(aes.decrypt_str(response_string));

		Json::Value reply;
		ss >> reply;

		if (reply["status"] == "failed")
		{
			//user not viable
			MessageBoxA(NULL, "failed sigs check", "failed.", 0);
			reply = {};
			return;
		}

		if(Utils::get_epoch_time_s() - reply["time"].asUInt() > 30)
		{
			MessageBoxA(NULL, "failed sigs check", "failed.", 0);
			reply = {};
			return;
		}


		const auto client = GetModuleHandleW(L"client_panorama.dll");
		const auto engine = GetModuleHandleW(L"engine.dll");
		const auto dx9api = GetModuleHandleW(L"shaderapidx9.dll");
		const auto vstdlib = GetModuleHandleW(L"vstdlib.dll");
		const auto vguimat_surf = GetModuleHandleW(L"vguimatsurface.dll");
		const auto vgui2 = GetModuleHandleW(L"vgui2.dll");
		const auto mat_sys = GetModuleHandleW(L"materialsystem.dll");

		auto engine_factory = get_module_factory(engine);
		auto client_factory = get_module_factory(client);
		auto valve_std_factory = get_module_factory(vstdlib);
		auto vgui_factory = get_module_factory(vguimat_surf);
		auto vgui2_factory = get_module_factory(vgui2);
		auto mat_sys_factory = get_module_factory(mat_sys);
		auto data_cache_factory = get_module_factory(GetModuleHandleW(L"datacache.dll"));
		auto v_phys_factory = get_module_factory(GetModuleHandleW(L"vphysics.dll"));
		auto input_sys_factory = get_module_factory(GetModuleHandleW(L"inputsystem.dll"));

		const auto user = ((HSteamUser(__cdecl*)())GetProcAddress(GetModuleHandle(L"steam_api.dll"), "SteamAPI_GetHSteamUser"))();
		const auto pipe = ((HSteamPipe(__cdecl*)())GetProcAddress(GetModuleHandle(L"steam_api.dll"), "SteamAPI_GetHSteamPipe"))();

		g_SteamClient = ((ISteamClient * (__cdecl*)())GetProcAddress(GetModuleHandle(L"steam_api.dll"), "SteamClient"))();
		g_SteamUser = g_SteamClient->GetISteamUser(user, pipe, "SteamUser019");
		g_SteamFriends = g_SteamClient->GetISteamFriends(user, pipe, "SteamFriends015");

		const auto vtable = *(const void***)(std::size_t)g_SteamFriends;

		g_CHLClient = get_interface<IBaseClientDLL>(client_factory, "VClient018");
		g_EntityList = get_interface<IClientEntityList>(client_factory, "VClientEntityList003");
		g_Prediction = get_interface<IPrediction>(client_factory, "VClientPrediction001");
		g_GameMovement = get_interface<CGameMovement>(client_factory, "GameMovement001");
		g_MdlCache = get_interface<IMDLCache>(data_cache_factory, "MDLCache004");
		g_EngineClient = get_interface<IVEngineClient>(engine_factory, "VEngineClient014");
		g_MdlInfo = get_interface<IVModelInfoClient>(engine_factory, "VModelInfoClient004");
		g_MdlRender = get_interface<IVModelRender>(engine_factory, "VEngineModel016");
		g_RenderView = get_interface<IVRenderView>(engine_factory, "VEngineRenderView014");
		g_EngineTrace = get_interface<IEngineTrace>(engine_factory, "EngineTraceClient004");
		g_DebugOverlay = get_interface<IVDebugOverlay>(engine_factory, "VDebugOverlay004");
		g_GameEvents = get_interface<IGameEventManager2>(engine_factory, "GAMEEVENTSMANAGER002");
		g_EngineSound = get_interface<IEngineSound>(engine_factory, "IEngineSoundClient003");
		g_MatSystem = get_interface<IMaterialSystem>(mat_sys_factory, "VMaterialSystem080");
		g_CVar = get_interface<ICvar>(valve_std_factory, "VEngineCvar007");
		g_VGuiPanel = get_interface<IPanel>(vgui2_factory, "VGUI_Panel009");
		g_VGuiSurface = get_interface<ISurface>(vgui_factory, "VGUI_Surface031");
		g_PhysSurface = get_interface<IPhysicsSurfaceProps>(v_phys_factory, "VPhysicsSurfaceProps001");

		g_GlobalVars = **(CGlobalVarsBase * **)((*(DWORD * *)g_CHLClient)[0] + 0x1B);
		g_ClientMode = **(IClientMode * **)((*(DWORD * *)g_CHLClient)[10] + 0x5);
		g_Input = *(CInput * *)((*(DWORD * *)g_CHLClient)[16] + 0x1);
		g_MoveHelper = **(IMoveHelper * **)(Utils::PatternScan(client, reply["move_helper"].asCString()).value() + 2);
		g_GlowObjManager = *(CGlowObjectManager * *)(Utils::PatternScan(client, reply["glow_obj_manager"].asCString()).value() + 3);
		g_ViewRender = *(IViewRender * *)(Utils::PatternScan(client, "A1 ? ? ? ? B9 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? FF 10").value() + 1);
		g_D3DDevice9 = **(IDirect3DDevice9 * **)(Utils::PatternScan(dx9api, "A1 ? ? ? ? 50 8B 08 FF 51 0C").value() + 1);
		g_ClientState = **(CClientState * **)(Utils::PatternScan(engine, reply["client_state"].asCString()).value() + 1);
		g_LocalPlayer = *(C_LocalPlayer*)(Utils::PatternScan(client, reply["local_player"].asCString()).value() + 2);
		g_RenderBeams = *(IViewRenderBeams * *)(Utils::PatternScan(client, reply["render_beams"].asCString()).value() + 1);

		std::string data_2 = "username=" + info_lp::username + "&steam_id=" + std::to_string(g_SteamUser->GetSteamID().ConvertToUint64());
		auto response = http_client.send_post(L"steam/save_steam_id.php", data_2); //we dont care about the response anyways
#ifndef _DEBUG
		VMProtectEnd();
#endif
	}

	void Dump()
	{
#define STRINGIFY_IMPL(s) #s
#define STRINGIFY(s)      STRINGIFY_IMPL(s)
#define PRINT_INTERFACE(name) Utils::ConsolePrint("%-20s: %p\n", STRINGIFY(name), name)

		PRINT_INTERFACE(g_CHLClient);
		PRINT_INTERFACE(g_EntityList);
		PRINT_INTERFACE(g_Prediction);
		PRINT_INTERFACE(g_GameMovement);
		PRINT_INTERFACE(g_MdlCache);
		PRINT_INTERFACE(g_EngineClient);
		PRINT_INTERFACE(g_MdlInfo);
		PRINT_INTERFACE(g_MdlRender);
		PRINT_INTERFACE(g_RenderView);
		PRINT_INTERFACE(g_EngineTrace);
		PRINT_INTERFACE(g_DebugOverlay);
		PRINT_INTERFACE(g_GameEvents);
		PRINT_INTERFACE(g_EngineSound);
		PRINT_INTERFACE(g_MatSystem);
		PRINT_INTERFACE(g_CVar);
		PRINT_INTERFACE(g_VGuiPanel);
		PRINT_INTERFACE(g_VGuiSurface);
		PRINT_INTERFACE(g_PhysSurface);
		PRINT_INTERFACE(g_RenderBeams);
		PRINT_INTERFACE(g_SteamUser);
		PRINT_INTERFACE(g_SteamFriends);
		PRINT_INTERFACE(g_SteamClient);
	}
}