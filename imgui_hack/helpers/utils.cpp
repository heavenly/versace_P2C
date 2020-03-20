#include "Utils.hpp"

#include <Windows.h>
#include <string>
#include <vector>
#include "../valve_sdk/csgostructs.hpp"
#include "Math.hpp"
#include <chrono>
#include "../valve_sdk/interfaces/ICvar.hpp"
#include "../valve_sdk/misc/Convar.hpp"

HANDLE _out = NULL, _old_out = NULL;
HANDLE _err = NULL, _old_err = NULL;
HANDLE _in = NULL, _old_in = NULL;

namespace Utils {
	unsigned int FindInDataMap(datamap_t *pMap, const char *name) {
		while (pMap) {
			for (int i = 0; i < pMap->dataNumFields; i++) {
				if (pMap->dataDesc[i].fieldName == NULL)
					continue;

				if (strcmp(name, pMap->dataDesc[i].fieldName) == 0)
					return pMap->dataDesc[i].fieldOffset[TD_OFFSET_NORMAL];

				if (pMap->dataDesc[i].fieldType == FIELD_EMBEDDED) {
					if (pMap->dataDesc[i].td) {
						unsigned int offset;

						if ((offset = FindInDataMap(pMap->dataDesc[i].td, name)) != 0)
							return offset;
					}
				}
			}
			pMap = pMap->baseMap;
		}

		return 0;
	}
	/*
	 * @brief Create console
	 *
	 * Create and attach a console window to the current process
	 */
	void AttachConsole()
	{
		_old_out = GetStdHandle(STD_OUTPUT_HANDLE);
		_old_err = GetStdHandle(STD_ERROR_HANDLE);
		_old_in = GetStdHandle(STD_INPUT_HANDLE);

		::AllocConsole() && ::AttachConsole(GetCurrentProcessId());

		_out = GetStdHandle(STD_OUTPUT_HANDLE);
		_err = GetStdHandle(STD_ERROR_HANDLE);
		_in = GetStdHandle(STD_INPUT_HANDLE);

		SetConsoleMode(_out,
			ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);

		SetConsoleMode(_in,
			ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS |
			ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE);
	}

	/*
	 * @brief Detach console
	 *
	 * Detach and destroy the attached console
	 */
	void DetachConsole()
	{
		if (_out && _err && _in) {
			FreeConsole();

			if (_old_out)
				SetStdHandle(STD_OUTPUT_HANDLE, _old_out);
			if (_old_err)
				SetStdHandle(STD_ERROR_HANDLE, _old_err);
			if (_old_in)
				SetStdHandle(STD_INPUT_HANDLE, _old_in);
		}
	}

	/*
	 * @brief Print to console
	 *
	 * Replacement to printf that works with the newly created console
	 */
	bool ConsolePrint(const char* fmt, ...)
	{
		if (!_out)
			return false;

		char buf[1024];
		va_list va;

		va_start(va, fmt);
		_vsnprintf_s(buf, 1024, fmt, va);
		va_end(va);
		
		return !!WriteConsoleA(_out, buf, static_cast<DWORD>(strlen(buf)), nullptr, nullptr);
	}

	/*
	 * @brief Blocks execution until a key is pressed on the console window
	 *
	 */
	char ConsoleReadKey()
	{
		if (!_in)
			return false;

		auto key = char{ 0 };
		auto keysread = DWORD{ 0 };

		ReadConsoleA(_in, &key, 1, &keysread, nullptr);

		return key;
	}


	/*
	 * @brief Wait for all the given modules to be loaded
	 *
	 * @param timeout How long to wait
	 * @param modules List of modules to wait for
	 *
	 * @returns See WaitForSingleObject return values.
	 */
	int WaitForModules(std::int32_t timeout, const std::initializer_list<std::wstring>& modules)
	{
		bool signaled[32] = { 0 };
		bool success = false;

		std::uint32_t totalSlept = 0;

		if (timeout == 0) {
			for (auto& mod : modules) {
				if (GetModuleHandleW(std::data(mod)) == NULL)
					return WAIT_TIMEOUT;
			}
			return WAIT_OBJECT_0;
		}

		if (timeout < 0)
			timeout = INT32_MAX;

		while (true) {
			for (auto i = 0u; i < modules.size(); ++i) {
				auto& module = *(modules.begin() + i);
				if (!signaled[i] && GetModuleHandleW(std::data(module)) != NULL) {
					signaled[i] = true;

					//
					// Checks if all modules are signaled
					//
					bool done = true;
					for (auto j = 0u; j < modules.size(); ++j) {
						if (!signaled[j]) {
							done = false;
							break;
						}
					}
					if (done) {
						success = true;
						goto exit;
					}
				}
			}
			if (totalSlept > std::uint32_t(timeout)) {
				break;
			}
			Sleep(10);
			totalSlept += 10;
		}

	exit:
		return success ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
	}

	/*
	 * @brief Scan for a given byte pattern on a module
	 *
	 * @param module    Base of the module to search
	 * @param signature IDA-style byte array pattern
	 *
	 * @returns Address of the first occurence
	 */
	std::optional<std::uint8_t*> PatternScan(void* module, const char* signature)
	{
		static auto pattern_to_byte = [](const char* pattern) {
			auto bytes = std::vector<int>{};
			auto start = const_cast<char*>(pattern);
			auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current) {
				if (*current == '?') {
					++current;
					if (*current == '?')
						++current;
					bytes.push_back(-1);
				}
				else {
					bytes.push_back(strtoul(current, &current, 16));
				}
			}
			return bytes;
		};

		auto dosHeader = (PIMAGE_DOS_HEADER)module;
		auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

		auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto patternBytes = pattern_to_byte(signature);
		auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

		auto s = patternBytes.size();
		auto d = patternBytes.data();

		for (auto i = 0ul; i < sizeOfImage - s; ++i) {
			bool found = true;
			for (auto j = 0ul; j < s; ++j) {
				if (scanBytes[i + j] != d[j] && d[j] != -1) {
					found = false;
					break;
				}
			}
			if (found) {
				return &scanBytes[i];
			}
		}
		return {};
	}

	/*
	 * @brief Set player clantag
	 *
	 * @param tag New clantag
	 */
	void SetClantag(const char* tag)
	{
		static auto prev_tag = tag;

		if (tag == prev_tag)
			return;

		prev_tag = tag;

		static auto clantag_sig = PatternScan(GetModuleHandleW(L"engine.dll"), "53 56 57 8B DA 8B F9 FF 15").value();
		static auto clantag_changed = (int(__fastcall*)(const char*, const char*))(clantag_sig);

		clantag_changed(tag, tag);
	}

	/*
	 * @brief Set player name
	 *
	 * @param name New name
	 */
	void SetName(const char* name)
	{
		static auto name_convar = g_CVar->FindVar("name");
		
		// Fix so we can change names how many times we want
		// This code will only run once because of `static`
		static auto do_once = (name_convar->SetValue("\nญญญ"), true) && (name_convar->m_fnChangeCallbacks.m_Size = 0);

		name_convar->SetValue(name);
	}

	/*
	 * @brief Reveal the ranks of all players on the server
	 *
	 */
	void RankRevealAll()
	{
		using ServerRankRevealAll = char(__cdecl*)(int*);

		static auto fnServerRankRevealAll = PatternScan(GetModuleHandleW(L"client_panorama.dll"), "55 8B EC 8B 0D ? ? ? ? 85 C9 75 ? A1 ? ? ? ? 68 ? ? ? ? 8B 08 8B 01 FF 50 ? 85 C0 74 ? 8B C8 E8 ? ? ? ? 8B C8 EB ? 33 C9 89 0D ? ? ? ? 8B 45 ? FF 70 ? E8 ? ? ? ? B0 ? 5D").value();

		int v[3] = { 0,0,0 };

		reinterpret_cast<ServerRankRevealAll>(fnServerRankRevealAll)(v);
	}

	bool line_goes_through_smoke(Vector start_pos, Vector end_pos)
	{
		static auto line_goes_through_smoke_fn = reinterpret_cast<bool(*)(Vector, Vector)>(PatternScan(GetModuleHandleA("client_panorama.dll"),
		                                                                                               "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0").value());
		return line_goes_through_smoke_fn(start_pos, end_pos);
	}

	void marquee(std::string& marquee)
	{
		std::string temp_string = marquee;
		marquee.erase(0, 1);
		marquee += temp_string[0];
	}

	int random_int(int min, int max)
	{
		return rand() % min + max;;
	}

	float random_float(float min, float max)
	{
		float random = ((float)rand()) / (float)RAND_MAX;

		float range = max - min;

		return (random*range) + min;
	}

	long long get_epoch_time()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	long long get_epoch_time_s()
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	void set_sky(std::string name)
	{
		static auto load_named_sky = reinterpret_cast<void(__fastcall*)(const char*)>(PatternScan(GetModuleHandleA("engine.dll"),
		                                                                                          "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45").value()); //55 8B EC 81 EC ? ? ? ? 56 57 8B F9 
		load_named_sky(name.c_str());
	}

	void copy_to_clipboard(const std::string s)
	{
		auto cc = [](const std::string val) -> void
		{
			OpenClipboard(0);
			EmptyClipboard();
			HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, val.size());
			if (!hg) {
				CloseClipboard();
				return;
			}
			memcpy(GlobalLock(hg), val.c_str(), val.size());
			GlobalUnlock(hg);
			SetClipboardData(CF_TEXT, hg);
			CloseClipboard();
			GlobalFree(hg);
		};
		cc(s);
	}

	Vector2D cubic_bezier(const Vector2D& a, const Vector2D& b, const Vector2D& c, const Vector2D& d, const float t) 
	{

		auto lerp = [](const Vector2D& a, const Vector2D& b, const float t)
		{
			Vector2D result;
			result.x = a.x + (b.x - a.x)*t;
			result.y = a.y + (b.y - a.y)*t;
			return result;
		};

		Vector2D ab = lerp(a, b, t);
		Vector2D bc = lerp(b, c, t);
		Vector2D cd = lerp(c, d, t);
		Vector2D abbc = lerp(ab, bc, t);
		Vector2D bccd = lerp(bc, cd, t);

		Vector2D result = lerp(abbc, bccd, t);
		return result;
	}

	static const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";


	static inline bool is_base64(unsigned char c) {
		return (isalnum(c) || (c == '+') || (c == '/'));
	}

	std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
		std::string ret;
		int i = 0;
		int j = 0;
		unsigned char char_array_3[3];
		unsigned char char_array_4[4];

		while (in_len--) {
			char_array_3[i++] = *(bytes_to_encode++);
			if (i == 3) {
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for (i = 0; (i < 4); i++)
					ret += base64_chars[char_array_4[i]];
				i = 0;
			}
		}

		if (i)
		{
			for (j = i; j < 3; j++)
				char_array_3[j] = '\0';

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

			for (j = 0; (j < i + 1); j++)
				ret += base64_chars[char_array_4[j]];

			while ((i++ < 3))
				ret += '=';

		}

		return ret;

	}

	std::string base64_decode(std::string const& encoded_string) {
		size_t in_len = encoded_string.size();
		int i = 0;
		int j = 0;
		int in_ = 0;
		unsigned char char_array_4[4], char_array_3[3];
		std::string ret;

		while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
			char_array_4[i++] = encoded_string[in_]; in_++;
			if (i == 4) {
				for (i = 0; i < 4; i++)
					char_array_4[i] = base64_chars.find(char_array_4[i]) & 0xff;

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (i = 0; (i < 3); i++)
					ret += char_array_3[i];
				i = 0;
			}
		}

		if (i) {
			for (j = 0; j < i; j++)
				char_array_4[j] = base64_chars.find(char_array_4[j]) & 0xff;

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

			for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
		}

		return ret;
	}
}