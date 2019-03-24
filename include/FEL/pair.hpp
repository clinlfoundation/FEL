#pragma once
#include "FEL/algorithm/move.hpp"

namespace fel{
	template<typename F, typename S>
	struct pair
	{
		F first;
		S second;

		template<typename vF, typename vS>
		pair(vF v1, vS v2)
		: first{v1}
		, second{v2}
		{}

		pair(const pair&) = default;
		pair(pair&&) = default;

		pair<F,S>& operator=(pair& oth)
		{
			first = oth.first;
			second = oth.second;
			return *this;
		}

		void operator=(pair&& oth)
		{
			first = fel::move(oth.first);
			second = fel::move(oth.second);
		}

		~pair() = default;
	};

}