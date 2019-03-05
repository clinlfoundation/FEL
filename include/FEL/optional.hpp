#pragma once
#include "type_traits.hpp"
#include "fel_config.hpp"
#include "FEL/exception.hpp"
#include "FEL/memory.hpp"

namespace fel{
	struct nullopt_t{};

	constexpr nullopt_t nullopt;

	template<typename T, bool B = std::is_final<T>::value || std::is_fundamental<T>::value>
	class optional;

	template<typename T>
	class optional<T,true>{
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

	template<typename T>
	class optional<T,false>{
		bool ready = false;
		void* ptr;
	public:
		constexpr optional(nullopt_t)
		: ready{false}
		{}

		constexpr optional(T& value)
		: ready{true}
		{
			ptr = (void*)new T(value);
		}

		constexpr optional(T&& value)
		: ready{true}
		{
			ptr = (void*)new T(std::move(value));
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
			return *reinterpret_cast<T*>(ptr);
		}
	};
}