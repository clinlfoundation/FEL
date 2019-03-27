#pragma once
#include <type_traits>
#include <cstdint>
#include "FEL/algorithm/tmp_manip.hpp"
namespace fel{
	template<typename T, typename Enable = void>
	struct hash;

	template<typename T>
	struct hash<T, std::enable_if_t<std::is_integral_v<T>, void>>{
		constexpr hash() = default;
		constexpr hash(const T&){};
		constexpr uint64_t operator()(const T& value) const 
		{
			return value;
		}
	};
	template<typename T>
	struct hash<T, std::enable_if_t<std::is_floating_point_v<T>, void>>{
		constexpr hash() = default;
		constexpr hash(const T&){};
		constexpr uint64_t operator()(const T& value) const 
		{
			if constexpr(sizeof(T)==2)
				return *(uint16_t*)&value;
			if constexpr(sizeof(T)==4)
				return *(uint32_t*)&value;
			if constexpr(sizeof(T)==8)
				return *(uint64_t*)&value;
		}
	};
	template<typename T>
	struct hash<T, std::enable_if_t<has_range_interface<T>::value, void>>{
		constexpr hash() = default;
		constexpr hash(const T&){};

		template<typename U>
		uint64_t subhash(const U& value) const
		{
			hash<U> child;
			return child(value);
		}

		uint64_t operator()(const T& value) const 
		{
			uint64_t cumul = 0;
			for(const auto& elem : value)
			{
				cumul++;
				cumul<<=1;
				cumul += subhash<>(elem);
			}
			return cumul;
		}
	};
}