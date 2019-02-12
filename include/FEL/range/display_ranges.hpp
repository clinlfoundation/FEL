#pragma once
#include "FEL/array.hpp"

namespace fel{

	template<typename it>
	it range_convert(uint64_t value, it b_iterator)
	{
		do
		{
			auto n = value%10;
			value -= n;
			value /= 10;
			*(--b_iterator) = '0' + n;
		}while(value!=0);
		return b_iterator;
	}

	template<typename it>
	it range_convert(int64_t value, it b_iterator)
	{
		bool sign = false;
		sign = value < 0;
		value *= -1 * sign;
		do
		{
			auto n = value%10;
			value -= n;
			value /= 10;
			*(--b_iterator) = '0' + n;
		}while(value!=0);
		if(sign)
			*(--b_iterator)='-';
		return b_iterator;
	}

	template<typename it>
	it range_convert(double value, it b_iterator)
	{
		bool sign = false;
		sign = value < 0;
		uint64_t* ptr = (uint64_t*)&value;
		*ptr = (*ptr & 0b01111111'11111111'11111111'11111111'11111111'11111111'11111111'11111111);
		uint64_t integral_part = value;
		uint64_t decimal_part = 0;
		value -= integral_part;
		uint64_t powers = 10;
		double rest;
		double ddecimal;
		do
		{
			ddecimal = value * powers;
			decimal_part = ddecimal;
			rest = ddecimal - decimal_part;
			powers *= 10;
		}while(rest != 0);

		b_iterator = range_convert(decimal_part, b_iterator);
		*(--b_iterator)='.';
		b_iterator = range_convert(integral_part, b_iterator);

		if(sign)
			*(--b_iterator)='-';
		return b_iterator;
	}

	class display_range{
		fel::array<char, 328> data;
	public:
		using associated_iterator = fel::array<char, 328>::associated_iterator;
		associated_iterator b_iterator;

		template<typename T>
		display_range(T value)
		: data{}
		, b_iterator{data.end()}
		{
			b_iterator=range_convert(value, b_iterator);
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