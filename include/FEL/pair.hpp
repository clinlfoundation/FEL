#pragma once
#include "FEL/algorithm/move.hpp"

namespace fel{
	template<typename F, typename S>
	struct pair
	{
		F first;
		S second;

		pair(F v1, S v2)
		{
			first = v1;
			second = v2;
		}

		pair(F& v1, S& v2)
		{
			first = v1;
			second = v2;
		}

		pair(F& v1, S&& v2)
		{
			first = v1;
			second = fel::move(v2);
		}

		pair(F&& v1, S&& v2)
		{
			first = fel::move(v1);
			second = fel::move(v2);
		}

		pair operator=(pair oth) = default;

		pair& operator=(pair& oth) = default;

		void operator=(pair&& oth)
		{
			first = fel::move(oth.first);
			second = fel::move(oth.second);
		}

		~pair() = default;
	};

}