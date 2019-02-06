#pragma once
#include <cstdint>
#include <cstddef>
#include <cstdlib>

namespace fel_config{
	namespace memory_module{
		constexpr void*(*memory_allocator)(std::size_t) = malloc;
		constexpr void(*memory_deallocator)(void*) = free;

		constexpr bool is_ok = 
			( memory_allocator != nullptr )
			&& ( memory_deallocator != nullptr );
	}

	constexpr bool has_exceptions = true;
}