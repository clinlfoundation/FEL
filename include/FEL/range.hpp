#pragma once
#include "FEL/algorithm/tmp_manip.hpp"
#include "FEL/algorithm/move.hpp"

namespace fel{
	template<typename it, bool Enable = std::is_same<typename it::difference_type,std::size_t>::value>
	class nameless_range;

	template<typename it>
	class nameless_range<it, false>
	{
		it _begin;
		it _end;

		friend nameless_range<it, true>;
	public:
		using associated_iterator = it;

		nameless_range() = delete;

		template<typename othT>
		nameless_range(const nameless_range<othT>& oth)
		: _begin(oth._begin)
		, _end(oth._end)
		{}

		nameless_range(it sbegin, it send)
		: _begin(sbegin)
		, _end(send)
		{}

		it begin() const
		{
			return _begin;
		}

		it end() const
		{
			return _end;
		}
	};

	template<typename it>
	class nameless_range<it, true> : public nameless_range<it, false>
	{
	public:
		nameless_range() = delete;

		template<typename othT>
		nameless_range(const nameless_range<othT>& oth)
		: nameless_range<it,false>(oth._begin,oth._end)
		{}

		nameless_range(it sbegin, it send)
		: nameless_range<it,false>(sbegin, send)
		{}

		std::size_t size() const
		{
			return this->_end-this->_begin;
		}
	};
}



