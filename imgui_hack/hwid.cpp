#include "hwid.h"
#define WIN32_LEAN_AND_MEAN        
#include <windows.h>     
#include <iostream>
#include <string>
#include <intrin.h>

UINT16 c_hwid::get_volume_hash()
{
	DWORD serial_num = 0;

	// Determine if this volume uses an NTFS file system.      
	GetVolumeInformation(L"c:\\", NULL, 0, &serial_num, NULL, NULL, NULL, 0);
	UINT16 hash = (UINT16)((serial_num + (serial_num >> 16)) & 0xFFFF);

	return hash;
}

UINT16 c_hwid::get_cpu_hash()
{
	int cpu_info[4] = { 0, 0, 0, 0 };
	__cpuid(cpu_info, 0);
	UINT16 hash = 0;
	UINT16* ptr = (UINT16*)(&cpu_info[0]);
	for (UINT32 i = 0; i < 8; i++)
		hash += ptr[i];

	return hash;
}

std::string c_hwid::get_hwid()
{
	return std::to_string(get_cpu_hash()) + std::to_string(get_volume_hash());
}