#pragma once
#include "FEL/range.hpp"

namespace fel{
	template<typename range_in, typename range_out>
	range_out copy(range_in src, range_out dest)
	{
		if(src.size()>dest.size())
			return dest;
		auto in = src.begin();
		auto in_close = src.end();
		auto out = dest.begin();
		while(in != in_close)
		{
			*(out++) = *(in++);
		}
		return nameless_range<typename range_out::associated_iterator>{out, dest.end()};
	}
}