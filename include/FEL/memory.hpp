#pragma once
#include "fel_config.hpp"
#include <type_traits>

namespace fel{
	template<class T, typename I = std::enable_if_t<fel_config::memory_module::is_ok,int>>
	class default_memory_allocator
	{
	public:
		using pointer_type = T*;
		using reference_type = T&;
		using const_pointer_type = const T*;
		using const_reference_type = const T&;

		pointer_type allocate(size_t sz)
		{
			return reinterpret_cast <pointer_type> (fel_config::memory_module::memory_allocator<fel_config::memory_module::memory_mode>(sizeof(T) * sz));
		}

		void deallocate(pointer_type ptr)
		{
			fel_config::memory_module::memory_deallocator<fel_config::memory_module::memory_mode>(ptr);
		}

		template<typename ...params>
		pointer_type construct(pointer_type ptr, params... args)
		{
			new(ptr) T(args...);
			return ptr;
		}

		void destroy(pointer_type v)
		{
			v->~T();
		}
	};
}

void* operator new(size_t sz)
{
	auto ptr = fel_config::memory_module::memory_allocator<fel_config::memory_module::memory_mode>(sz);
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
	fel_config::memory_module::memory_deallocator<fel_config::memory_module::memory_mode>(ptr);
}