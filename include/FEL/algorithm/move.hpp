#pragma once
#include "FEL/algorithm/tmp_manip.hpp"
#include "FEL/range.hpp"

namespace fel{
	template<typename T>
	typename fel::remove_reference<T>::type&& move(T&& value)
	{
		return (typename fel::remove_reference<T>::type&&)value;
	}

	template <class T> 
	constexpr T&& forward(typename fel::remove_reference<T>::type& t) noexcept
	{
		return static_cast<T&&>(t);
	}
	template <class T> 
	constexpr T&& forward(typename fel::remove_reference<T>::type&& t) noexcept
	{
		static_assert(!std::is_lvalue_reference_v<T>,"bad forward of rvalue as lvalue");
		return static_cast<T&&>(t);
	}

	template<typename range_in, typename range_out>
	nameless_range<typename range_out::associated_iterator> move(range_in src, range_out dest)
	{
		if(src.size()>dest.size())
			return nameless_range<typename range_out::associated_iterator>(dest.begin(), dest.end());
		auto in = src.begin();
		auto in_close = src.end();
		auto out = dest.begin();
		while(in != in_close)
		{
			*(out++) = fel::move(*(in++));
		}
		return nameless_range<typename range_out::associated_iterator>{out, dest.end()};
	}

	template<typename T, typename range_in, typename range_out>
	nameless_range<typename range_out::associated_iterator> move_uninitialized(range_in src, range_out dest)
	{
		if(src.size()>dest.size())
			return nameless_range<typename range_out::associated_iterator>(dest.begin(), dest.end());
		auto in = src.begin();
		auto in_close = src.end();
		auto out = dest.begin();
		while(in != in_close)
		{
			new(&*(out++)) T{fel::move(*(in++))};
		}
		return nameless_range<typename range_out::associated_iterator>{out, dest.end()};
	}
}