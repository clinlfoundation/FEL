#pragma once

namespace fel{
	template<typename T, typename U, typename ...rest>
	constexpr T max(T first, U second, rest... args)
	{
		if constexpr (sizeof...(args) == 0)
		{
			return first > second ? first : second;
		}
		else
		{
			return max(first > second ? first : second, args...);
		}
	}

	template<typename T, typename U, typename ...rest>
	constexpr T min(T first, U second, rest... args)
	{
		if constexpr (sizeof...(args) == 0)
		{
			return first < second ? first : second;
		}
		else
		{
			return max(first < second ? first : second, args...);
		}
	}
}