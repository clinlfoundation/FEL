#pragma once
#include "fel_config.hpp"
#include <type_traits>

namespace fel{
	template<typename I = std::enable_if_t<fel_config::memory_module::is_ok,int>>
	class default_memory_allocator
	{
	public:
		void* allocate(size_t sz)
		{
			return fel_config::memory_module::memory_allocator<fel_config::memory_module::memory_mode>(sz);
		}

		void deallocate(void* ptr)
		{
			fel_config::memory_module::memory_deallocator<fel_config::memory_module::memory_mode>(ptr);
		}
	};

	
}

void* operator new(size_t sz)
{
	auto ptr = fel::default_memory_allocator{}.allocate(sz);
	if constexpr (fel_config::has_exceptions)
	{
		if(!ptr)
		{
			throw fel::bad_alloc{};
		}
	}
	return ptr;
}

void operator delete (void* ptr) noexcept
{
	fel::default_memory_allocator{}.deallocate(ptr);
}