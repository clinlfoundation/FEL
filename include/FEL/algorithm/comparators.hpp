#pragma once
#include "FEL/algorithm/move.hpp"

namespace fel{
	template<typename T>
	struct identity
	{
		template<typename U>
		constexpr bool operator() (U value)
		{
			return fel::forward(value);
		}
	};

	template<typename T1, typename T2>
	struct equals
	{
		constexpr bool operator() (const T1& lhs, const T2& rhs)
		{
			return lhs == rhs;
		}
	};
}