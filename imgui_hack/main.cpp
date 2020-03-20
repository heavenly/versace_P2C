#pragma once
#include "valve_sdk/sdk.hpp"
#include "helpers/utils.hpp"
#include "helpers/input.hpp"
#include "local_info.h"
#include "hooks.hpp"
#include "menu.hpp"
#include "options.hpp"
#include "render.h"
#include <Iphlpapi.h>
#include "json.h"
#include "hwid.h"
#include "notification.h"
#include "aes.h"
#include "http_client.h"
#include <filesystem>
#include <VMProtectSDK.h>

long __stdcall exception_filter(struct _EXCEPTION_POINTERS* ExceptionInfo);

#ifdef _DEBUG
#define FUNCTION_GUARD info_lp::last_function_call = __FUNCTION__
#endif

DWORD WINAPI OnDllAttach(LPVOID base)
{
	if (Utils::WaitForModules(10000, { L"client_panorama.dll", L"engine.dll", L"shaderapidx9.dll", L"serverbrowser.dll" }) == WAIT_TIMEOUT)
		return FALSE;

#ifndef _DEBUG
	VMProtectBeginUltra("main.cpp");
#endif

	auto get_string_reg_key = [=](HKEY key, const std::string & value_name, std::string & value, const std::string & default_value) -> LONG
	{
		value = default_value;

		WCHAR buffer[512];
		DWORD buffer_size = sizeof(buffer);

		const std::wstring value_ws(value_name.begin(), value_name.end());

		const ULONG error = RegQueryValueEx(key, value_ws.c_str(), 0, NULL, (LPBYTE)buffer, &buffer_size);

		if (ERROR_SUCCESS == error)
		{
			std::wstring ws(buffer);
			std::string str(ws.begin(), ws.end());

			const c_aes aes = c_aes();
			value = aes.decrypt_str(str); //username is encrypted
		}

		return error;
	};
	HKEY key;
	std::string subkey = "SOFTWARE\\VER$ACE";
	std::wstring v(subkey.begin(), subkey.end());

	RegOpenKeyEx(HKEY_CURRENT_USER, v.c_str(), 0, KEY_ALL_ACCESS, &key);
	get_string_reg_key(key, "username", info_lp::username, "unknown");

	auto hw = c_hwid();
	info_lp::hwid = hw.get_hwid();

	SetUnhandledExceptionFilter(exception_filter);

	c_aes aes = c_aes();
	c_http_client http_client = c_http_client();

	std::string data = "username=" + aes.encrypt_str(info_lp::username) + "&hwid=" + aes.encrypt_str(info_lp::hwid);
	std::string response_string = http_client.send_post(L"authentication/dll_auth.php", data);

	std::stringstream ss;
	ss.str(aes.decrypt_str(response_string));

	Json::Value reply;
	ss >> reply;

	if (reply["status"] != "successful")
	{
		MessageBoxA(NULL, "failed.", "failed.", 0);
		return FALSE;
	}

	if(Utils::get_epoch_time_s() - reply["time"].asUInt() > 30)
	{
		MessageBoxA(NULL, "failed.", "failed.", 0);
		return FALSE;
	}
#ifndef _DEBUG
	VMProtectEnd();
#endif
	try {
		Interfaces::Initialize();
		Interfaces::Dump();
		NetvarSys::Get().Initialize();
		InputSys::Get().Initialize();
		Render::Get().Initialize();
		Menu::Get().Initialize();
		Hooks::Initialize();
		
		// Menu Toggle
		InputSys::Get().RegisterHotkey(VK_INSERT, []() {
			Menu::Get().Toggle();
			});

		const auto check_and_create = [](std::string dir) -> void
		{
			if (!std::filesystem::exists(dir))
				std::filesystem::create_directory(dir);
		};

		check_and_create("C:\\VER$ACE");
		check_and_create("C:\\VER$ACE\\friends_list");
		check_and_create("C:\\VER$ACE\\configs");
		check_and_create("C:\\VER$ACE\\scripts");
		
		
		Utils::ConsolePrint("Finished.\n");
		push_notifications::Push("welcome to VER$ACE " + info_lp::username);
		while (!g_Unload)
			Sleep(1000);

		FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
	}
	catch (const std::exception & ex) {
		Utils::ConsolePrint("Error occured during initialization:\n");
		Utils::ConsolePrint("%s\n", ex.what());
		Utils::ConsolePrint("Press any key to exit.\n");
		Utils::ConsoleReadKey();
		Utils::DetachConsole();
		
		FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
	}
}

BOOL WINAPI OnDllDetach()
{
#ifdef _DEBUG
	Utils::DetachConsole();
#endif

	Hooks::Shutdown();

	Menu::Get().Shutdown();
	return TRUE;
}

BOOL WINAPI DllMain(
	_In_      HINSTANCE hinstDll,
	_In_      DWORD     fdwReason,
	_In_opt_  LPVOID    lpvReserved
)
{
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDll);
		CreateThread(nullptr, 0, OnDllAttach, hinstDll, 0, nullptr);
		return TRUE;
	case DLL_PROCESS_DETACH:
		if (lpvReserved == nullptr)
			return OnDllDetach();
		return TRUE;
	default:
		return TRUE;
	}
}
	
long __stdcall exception_filter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	int exception_top_code = ExceptionInfo->ExceptionRecord->ExceptionCode;
	int exception_sub_reason = ExceptionInfo->ExceptionRecord->ExceptionInformation[0];
	int exception_address = ExceptionInfo->ExceptionRecord->ExceptionInformation[1];

	std::string exception_cause = "unknown";
	std::string exception_detail = "unknown";
	switch (exception_top_code)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		exception_cause = "caused by ACCESS_VIOLATION";
		if (exception_sub_reason == 0)
		{
			// bad read
			exception_detail = "failed to read" + std::to_string(exception_address);
		}
		else if (exception_sub_reason == 1)
		{
			// bad write
			exception_detail = "failed to write " + std::to_string(exception_address);
		}
		else if (exception_sub_reason == 8)
		{
			// user-mode data execution prevention (DEP)
			exception_detail = "DEP " + std::to_string(exception_address);
		}
		else
		{
			// unknown, shouldn't happen
			exception_detail = "unknown access violation, " + std::to_string(exception_address);
		}
		break;

	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		exception_cause = "exceeded array bounds";
		break;
		
	default:
		exception_cause = "unknown";
	}
	
	std::string eip_addr = std::to_string(ExceptionInfo->ContextRecord->Eip);


	std::string data = "username=" + info_lp::username + "&last_func=" + info_lp::last_function_call + "&exc_cause=" + exception_cause + "&exc_detail=" + exception_detail + "&eip=" + eip_addr;
	c_http_client http_client = c_http_client();
	auto response = http_client.send_post(L"error_logging/dll_exception.php", data); //we dont really care about the response
	return EXCEPTION_CONTINUE_SEARCH;
}