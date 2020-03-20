#include "http_client.h"

std::string c_http_client::send_post(std::wstring path, std::string data)
{
	WinHttpClient client(base_url + path);
	client.SetUserAgent(user_agent);
	client.SetAdditionalDataToSend((BYTE*)data.c_str(), data.size());

	wchar_t size[50] = L"";
	swprintf_s(size, L"%d", data.size());
	std::wstring headers = L"Content-Length: ";
	headers += size;
	headers += L"\r\nContent-Type: application/x-www-form-urlencoded\r\n";
	client.SetAdditionalRequestHeaders(headers);
	client.SendHttpRequest(L"POST");

	std::wstring response_header = client.GetResponseHeader();
	std::wstring response_content = client.GetResponseContent();
	std::string response_content_string(response_content.begin(), response_content.end());

	return response_content_string;
}

std::string c_http_client::send_get(std::wstring path)
{
	WinHttpClient client(base_url + path);
	client.SetUserAgent(user_agent);
	client.SendHttpRequest(L"GET");

	std::wstring response_header = client.GetResponseHeader();
	std::wstring response_content = client.GetResponseContent();
	std::string response_content_string(response_content.begin(), response_content.end());

	return response_content_string;
}
