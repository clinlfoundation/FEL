#pragma once
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <type_traits>

namespace fel_config{
	namespace memory_module{
		enum class memory_mode_t{
			other,
			clib,
			buffer,
			arena_buffer
		};

		constexpr memory_mode_t memory_mode = memory_mode_t::clib;


		template<memory_mode_t T = memory_mode_t::other>
		constexpr void*(*memory_allocator)(std::size_t)=nullptr;
		template<memory_mode_t T = memory_mode_t::other>
		constexpr void(*memory_deallocator)(void*)=nullptr;

		// C Standard library memory usage
		template<>
		constexpr void*(*memory_allocator<memory_mode_t::clib>)(std::size_t) = malloc;
		template<>
		constexpr void(*memory_deallocator<memory_mode_t::clib>)(void*) = free;

		// Buffer memory usage only
		template<>
		constexpr void*(*memory_allocator<memory_mode_t::buffer>)(std::size_t) = nullptr;
		template<>
		constexpr void(*memory_deallocator<memory_mode_t::buffer>)(void*) = nullptr;

		// Buffer of arena memory usage
		template<>
		constexpr void*(*memory_allocator<memory_mode_t::arena_buffer>)(std::size_t) = nullptr;
		template<>
		constexpr void(*memory_deallocator<memory_mode_t::arena_buffer>)(void*) = nullptr;

		constexpr bool is_ok = 
			( memory_allocator<memory_mode> != nullptr )
			&& ( memory_deallocator<memory_mode> != nullptr );
	}

	constexpr bool has_exceptions = true;
}