#include "helpers/vfunc_hook.hpp"
std::optional<uintptr_t*> vfunc_hook::search_free_data_page(const char* module_name, const std::size_t vmt_size)
{
	const static auto check_data_section = [](const LPCVOID address, const std::size_t vmt_size)
	{
		const DWORD DataProtection = (PAGE_EXECUTE_READWRITE | PAGE_READWRITE);
		MEMORY_BASIC_INFORMATION mbi = { 0 };
		const auto mbi_size = sizeof(mbi);
		const auto valid_vquery_size = VirtualQuery(address, &mbi, mbi_size) == mbi_size;
		const auto valid_mbi_values = mbi.AllocationBase && mbi.BaseAddress;
		const auto valid_mbi_etc = mbi.State == MEM_COMMIT && !(mbi.Protect & PAGE_GUARD) && mbi.Protect != PAGE_NOACCESS;
		if (valid_vquery_size && valid_mbi_values && valid_mbi_etc)
		{
				return mbi.Protect & DataProtection && mbi.RegionSize >= vmt_size;
		}
		return false;
	};

	const auto module_addr = GetModuleHandleA(module_name);

	if (module_addr == nullptr)
		return {};

	const auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER> (module_addr);
	const auto nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS> (reinterpret_cast<std::uint8_t*>(module_addr) + dos_header->e_lfanew);

	const auto module_end = reinterpret_cast<std::uintptr_t>(module_addr) + nt_headers->OptionalHeader.SizeOfImage - sizeof(std::uintptr_t);

	for (auto current_address = module_end; current_address > DWORD(module_addr); current_address -= sizeof(std::uintptr_t))
	{
		if (*reinterpret_cast<std::uintptr_t*>(current_address) == 0 && check_data_section(reinterpret_cast<LPCVOID>(current_address), vmt_size))
		{
			auto is_good_vmt = true;
			auto page_count = 0u;

			for (; page_count < vmt_size && is_good_vmt; page_count += sizeof(std::uintptr_t))
			{
				if (*reinterpret_cast<std::uintptr_t*>(current_address + page_count) != 0)
					is_good_vmt = false;
			}

			if (is_good_vmt && page_count >= vmt_size)
				return reinterpret_cast<uintptr_t*>(current_address);
		}
	}
	return {};
}

vfunc_hook::vfunc_hook()
	: class_base(nullptr), vftbl_len(0), new_vftbl(nullptr), old_vftbl(nullptr)
{
}
vfunc_hook::vfunc_hook(void* base)
	: class_base(base), vftbl_len(0), new_vftbl(nullptr), old_vftbl(nullptr)
{
}

vfunc_hook::~vfunc_hook()
{
	unhook_all();
	if (was_allocated)
		delete[] new_vftbl;
}

bool vfunc_hook::setup(void* base, const char* module_name)
{
	if (base)
		class_base = base;

	if (!class_base)
		return false;

	old_vftbl = *static_cast<std::uintptr_t**>(class_base);
	
	if (!old_vftbl)
		return false;
	
	vftbl_len = estimate_vftbl_length(old_vftbl) * sizeof(std::uintptr_t);

	if (vftbl_len == 0)
		return false;
	
	if (module_name != nullptr)
	{
		const auto free_page_result_opt = search_free_data_page(module_name, vftbl_len + sizeof(std::uintptr_t));
		if (!free_page_result_opt.has_value())
			return false;

		if (free_page_result_opt.value() == nullptr)
			return false;

		new_vftbl = free_page_result_opt.value();
		std::memset(new_vftbl, NULL, vftbl_len + sizeof(std::uintptr_t));
		std::memcpy(&new_vftbl[1], old_vftbl, vftbl_len);
		
		try {
			const auto guard = detail::protect_guard{ class_base, sizeof(std::uintptr_t), PAGE_READWRITE };
			new_vftbl[0] = old_vftbl[-1];
			*static_cast<std::uintptr_t**>(class_base) = &new_vftbl[1];
			was_allocated = false;
		}
		catch (...) {
			delete[] new_vftbl;
			return false;
		}
		this->mdle_name = module_name;
	}
	else
	{
		new_vftbl = new std::uintptr_t[vftbl_len + 1]();
		std::memcpy(&new_vftbl[1], old_vftbl, vftbl_len);
		try {
			const auto guard = detail::protect_guard{ class_base, sizeof(std::uintptr_t), PAGE_READWRITE };
			new_vftbl[0] = old_vftbl[-1];
			*static_cast<std::uintptr_t**>(class_base) = &new_vftbl[1];
			was_allocated = true;
		}
		catch (...) {
			delete[] new_vftbl;
			return false;
		}
	}


	return true;
}

std::size_t vfunc_hook::estimate_vftbl_length(std::uintptr_t* vftbl_start)
{
	const auto valid_code_ptr = [](const uintptr_t addr) -> bool {
		if (!addr)
			return false;

		MEMORY_BASIC_INFORMATION mbi;
		if (!VirtualQuery(reinterpret_cast<const void*>(addr), &mbi, sizeof mbi))
			return false;

		if (!(mbi.Protect & PAGE_EXECUTE_READWRITE || mbi.Protect & PAGE_EXECUTE_READ))
			return false;

		return true;
	};

	size_t size = 0;

	while (valid_code_ptr(vftbl_start[size]))
		++size;

	return size;
}