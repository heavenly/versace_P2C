#pragma once

#define NOMINMAX
#include <Windows.h>
#include <string>
#include <initializer_list>
#include <optional>
#include "../valve_sdk/sdk.hpp"
#include "../datamap.hpp"

namespace Utils {
	unsigned int FindInDataMap(datamap_t * pMap, const char * name);
    /*
     * @brief Create console
     *
     * Create and attach a console window to the current process
     */
	void AttachConsole();

    /*
     * @brief Detach console
     *
     * Detach and destroy the attached console
     */
    void DetachConsole();

    /*
     * @brief Print to console
     *
     * Replacement to printf that works with the newly created console
     */
    bool ConsolePrint(const char* fmt, ...);
    
    /*
     * @brief Blocks execution until a key is pressed on the console window
     *
     */
    char ConsoleReadKey();

    /*
     * @brief Wait for all the given modules to be loaded
     *
     * @param timeout How long to wait
     * @param modules List of modules to wait for
     *
     * @returns See WaitForSingleObject return values.
     */
    int WaitForModules(std::int32_t timeout, const std::initializer_list<std::wstring>& modules);

    /*
     * @brief Scan for a given byte pattern on a module
     *
     * @param module    Base of the module to search
     * @param signature IDA-style byte array pattern
     *
     * @returns Address of the first occurence
     */
    std::optional<std::uint8_t*> PatternScan(void* module, const char* signature);

    /*
     * @brief Set player clantag
     *
     * @param tag New clantag
     */
    void SetClantag(const char* tag);

    /*
     * @brief Set player name
     *
     * @param name New name
     */
    void SetName(const char* name);

    /*
     * @brief Reveal the ranks of all players on the server
     *
     */
    void RankRevealAll();

	bool line_goes_through_smoke(Vector start_pos, Vector end_pos);

	void marquee(std::string& marquee);

	int random_int(int min, int max);
	float random_float(float min, float max);
	long long get_epoch_time();
	long long get_epoch_time_s();
	void set_sky(std::string name);
	void copy_to_clipboard(const std::string s);
	Vector2D cubic_bezier(const Vector2D& a, const Vector2D& b, const Vector2D& c, const Vector2D& d, const float t);
	template<class T>
	T* find_hud_element(const char * name)
	{
		static auto pointer_this = *reinterpret_cast<DWORD**>(Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08").value() + 1);
		static auto find_hud_element_fn = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28").value());

		return (T*)find_hud_element_fn(pointer_this, name);
	}

	std::string base64_encode(unsigned char const*, unsigned int len);
	std::string base64_decode(std::string const& s);
}
