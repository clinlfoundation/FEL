#pragma once
#include <FEL/algorithm/tmp_manip.hpp>
#include <type_traits>
#include <new>
#include "fel_config.hpp"
#include "FEL/exception.hpp"


namespace fel{

	template< typename Enable, typename ...T>
	class variant;

	template<typename ...T>
	class variant<typename std::enable_if<all_of_fixed_size<T...>::value,int>::type, T...>{
		std::size_t index = std::numeric_limits<std::size_t>::max();
		char buffer[max_size<T...>];
	public:
		template<typename U, typename std::enable_if<list_contains_class<U,T...>::value,int>::type>
		constexpr variant(U& value)
		: index{r_index_of<U, T...>::value}
		{
			new(buffer) U(value);
		}

		template<typename U, typename std::enable_if<list_contains_class<U,T...>::value,int>::type>
		constexpr variant(U&& value)
		: index{r_index_of<U, T...>::value}
		{
			new(buffer) U(std::move(value));
		}

		template<typename U>
		constexpr U& value()
		{
			if constexpr (fel_config::has_exceptions)
			{
				if(r_index_of<U, T...>::value != index)
				{
					throw bad_variant_access<U>{};
				}
			}
			return *reinterpret_cast<U*>(buffer);
		}

		template<typename U>
		constexpr U& is_a()
		{
			if(r_index_of<U, T...>::value == index)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	};
}