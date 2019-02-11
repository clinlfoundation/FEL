#pragma once
#include "FEL/array.hpp"

namespace fel{


	template<typename T>
	class display_range{
		fel::array<char, 20> data;
	public:
		using associated_iterator = fel::array<char, 24>::associated_iterator;
		associated_iterator b_iterator;

		display_range(T value)
		: b_iterator{data.end()}
		{
			bool sign = false;
			if constexpr (std::is_signed_v<T>)
			{
				sign = value < 0;
				value *= -1 * sign;
			}
			do
			{
				auto n = value%10;
				value -= n;
				value /= 10;
				*(--b_iterator) = '0' + n;
			}while(value!=0);
			if constexpr (std::is_signed_v<T>)
				if(sign)
					*(--b_iterator)='-';
		}

		constexpr associated_iterator begin() const
		{
			return b_iterator;
		}

		constexpr associated_iterator end() const
		{
			return data.end();
		}

		constexpr size_t size() const
		{
			return data.end()-b_iterator;
		}
	};
}