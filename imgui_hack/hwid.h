#pragma once
#include <windows.h>
#include <string>

class c_hwid
{
public:
	std::string get_hwid();
private:
	UINT16 get_volume_hash();
	UINT16 get_cpu_hash();
};