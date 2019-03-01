#pragma once
#include "FEL/range.hpp"
#include "FEL/iterator.hpp"

namespace fel{
	template<typename range_in, typename range_out>
	range_out copy(range_in src, range_out dest)
	{
		if constexpr (range_in::associated_iterator::iterator_type != iterator_type_t::lazy_iterator)
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
		else
		{
			auto in = src.begin();
			auto out = dest.begin();
			auto out_close = dest.end();
			while(out != out_close)
			{
				*(out++) = *(in++);
			}
			return nameless_range<typename range_out::associated_iterator>{dest.end(), dest.end()};
		}
	}
}