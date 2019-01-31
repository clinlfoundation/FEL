#pragma once
#include <type_traits>
#include <new>
#include "fel_config.hpp"
#include "FEL/exception.hpp"

namespace fel{
	struct nullopt_t{};

	constexpr nullopt_t nullopt;

	template<typename T, typename U = typename std::enable_if<std::is_final<T>::value || std::is_fundamental<T>::value,int>::type>
	class optional{
		bool ready = false;
		char buffer[sizeof(T)];
	public:
		constexpr optional(nullopt_t)
		: ready{false}
		{}

		constexpr optional(T& value)
		: ready{true}
		{
			new(buffer) T(value);
		}

		constexpr optional(T&& value)
		: ready{true}
		{
			new(buffer) T(std::move(value));
		}

		constexpr bool has_value()
		{
			return ready;
		}

		constexpr T& value()
		{
			if constexpr (fel_config::has_exceptions)
			{
				if(!ready)
				{
					throw bad_optional{};
				}
			}
			return *reinterpret_cast<T*>(buffer);
		}
	};
}