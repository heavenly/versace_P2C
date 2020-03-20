#pragma once
#define NOMINMAX
#include <Windows.h>

#include <cstdint>
#include <stdexcept>
#include <optional>

namespace detail
{
	class protect_guard  // NOLINT(hicpp-special-member-functions)
	{
	public:
		protect_guard(void* base, const size_t len, const std::uint32_t flags)
		{
			_base = base;
			_length = len;
			if (!VirtualProtect(base, len, flags, PDWORD(&_old)))
				throw std::runtime_error("Region protection failure.");
		}
		
		~protect_guard()
		{
			VirtualProtect(_base, _length, _old, PDWORD(&_old));
		}

	private:
		void* _base;
		size_t        _length;
		std::uint32_t _old{};
	};
}

class vfunc_hook  // NOLINT(cppcoreguidelines-special-member-functions)
{
public:
	static std::optional<uintptr_t*> search_free_data_page(const char* module_name, const std::size_t vmt_size);
	vfunc_hook();
	explicit vfunc_hook(void* base);
	~vfunc_hook();

	bool setup(void* base, const char* module_name = nullptr);
	
	template<typename T>
	void hook_index(const int index, const T fun)
	{
		//assert(index >= 0 && index <= (int)vftbl_len);
		if (index < 0 || index > int(vftbl_len))
			return;
		
		new_vftbl[index + 1] = reinterpret_cast<std::uintptr_t>(fun);
	}
	
	void unhook_index(const int index) const
	{
		new_vftbl[index] = old_vftbl[index];
	}
	
	void unhook_all()
	{
		try {
			if (old_vftbl != nullptr) {
				const auto guard = detail::protect_guard{ class_base, sizeof(std::uintptr_t), PAGE_READWRITE };
				*static_cast<std::uintptr_t**>(class_base) = old_vftbl;
				old_vftbl = nullptr;
			}
		}
		catch (...) {
		}
	}

	template<typename T>
	T get_original(const int index)
	{
		return T(old_vftbl[index]);
	}

	bool is_valid() const
	{
		return new_vftbl != nullptr && old_vftbl != nullptr && class_base != nullptr;
	}

	const char* mdle_name;
	
private:

	static inline std::size_t estimate_vftbl_length(std::uintptr_t* vftbl_start);

	// ReSharper disable CppInconsistentNaming
	void*			class_base;
	std::size_t     vftbl_len;
	std::uintptr_t* new_vftbl;
	std::uintptr_t* old_vftbl;
	LPCVOID         search_base{};
	bool			was_allocated = false;
	// ReSharper restore CppInconsistentNaming
};