#pragma once
#include "FEL/algorithm/tmp_manip.hpp"

namespace fel{
	template<typename T>
	typename fel::remove_reference<T>::type&& move(T&& value)
	{
		return (typename fel::remove_reference<T>::type&&)value;
	}
/*
	template<typename range_in, typename range_out>
	range_out move(range_in input, range_out output)
	{
		
	}
*/
}