#pragma once
#include <type_traits>
#include <typeinfo>
#include <cstddef>
#include <limits>
#include "FEL/algorithm/min_max.hpp"


namespace fel{
	template<bool cond, typename T, typename U>
	struct either
	{
	};

	template<typename T, typename U>
	struct either<true,T,U>
	{
		typedef T type;
	};

	template<typename T, typename U>
	struct either<false,T,U>
	{
		typedef U type;
	};

	template<bool first, bool ...list>
	struct constexpr_all_of
	{
		static constexpr bool value = first && constexpr_all_of<list...>::value;
	};

	template<bool first>
	struct constexpr_all_of<first>
	{
		static constexpr bool value = first;
	};

	template<bool first, bool ...list>
	struct constexpr_any_of
	{
		static constexpr bool value = first || constexpr_any_of<list...>::value;
	};

	template<bool first>
	struct constexpr_any_of<first>
	{
		static constexpr bool value = first;
	};

	template<typename T>
	constexpr bool is_fixed_size()
	{
		return std::is_final<T>::value 
			|| std::is_fundamental<T>::value;
	}

	template<typename T, typename ...rest>
	struct all_of_fixed_size
	{
		static constexpr bool value = is_fixed_size<T>() && all_of_fixed_size<rest...>::value;
	};

	template<typename T>
	struct all_of_fixed_size<T>
	{
		static constexpr bool value = is_fixed_size<T>();
	};

	template<typename Univ, typename T, typename ...rest>
	struct list_contains_class
	{
		static constexpr bool value = (
			std::is_same<T, Univ>::value
		) || list_contains_class<Univ, rest...>::value;
	};

	template<typename Univ, typename T>
	struct list_contains_class<Univ, T>
	{
		static constexpr bool value = std::is_same<T, Univ>::value;
	};

	template<typename Univ, typename T, typename ...rest>
	struct r_index_of
	{
		static constexpr std::size_t value = std::is_same<T, Univ>::value ? sizeof...(rest) : r_index_of<Univ, rest...>::value;
	};

	template<typename Univ, typename T>
	struct r_index_of<Univ, T>
	{
		static constexpr std::size_t value = std::is_same<T, Univ>::value ? 0 : std::numeric_limits<std::size_t>::max();
	};

	template<typename T, typename U, typename ...rest>
	constexpr std::size_t max_size()
	{
		if constexpr (sizeof...(rest) == 0)
		{
			return max(sizeof(T),sizeof(U));
		}
		else
		{
			return max_size<
				either<
					( sizeof(T) > sizeof(U) ), 
					T, 
					U
				>::type, 
			rest...>();
		}
	}

	template<typename T>
	struct remove_reference
	{
		using type = T;
	};

	template<typename T>
	struct remove_reference<T&>
	{
		using type = T;
	};

	template<typename T>
	struct remove_reference<T&&>
	{
		using type = T;
	};
}