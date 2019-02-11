#pragma once
#include "FEL/algorithm/tmp_manip.hpp"
#include "FEL/range.hpp"

namespace fel{
	template<typename T>
	typename fel::remove_reference<T>::type&& move(T&& value)
	{
		return (typename fel::remove_reference<T>::type&&)value;
	}

	template<typename range_in, typename range_out>
	range_out move(range_in src, range_out dest)
	{
		if(src.size()>dest.size())
			return dest;
		auto in = src.begin();
		auto in_close = src.end();
		auto out = dest.begin();
		while(in != in_close)
		{
			*(out++) = fel::move(*(in++));
		}
		return nameless_range<typename range_out::associated_iterator>{out, dest.end()};
	}
}