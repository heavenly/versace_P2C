#pragma once
#include <string>
#include <WinHttpClient.h>

class c_http_client
{
private:
	std::wstring user_agent = L"VER$ACE";
	std::wstring base_url = L"https://versacehack.xyz/";
public:
	std::string send_post(std::wstring path, std::string data);
	std::string send_get(std::wstring path);
};