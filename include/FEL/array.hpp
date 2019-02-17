#pragma once
#include <FEL/buffer.hpp>

namespace fel{
	template<typename T, std::size_t sz>
	class array{
		T ary[sz];
	public:
		using associated_iterator = typename buffer<T>::associated_iterator;

		constexpr T& operator[] (size_t off)
		{
			return ary[off];
		}

		constexpr size_t size() const
		{
			return sz;
		}

		constexpr associated_iterator begin() const
		{
			return fel::buffer<T>{(T*)ary, (T*)ary+sz}.begin();
		}

		constexpr associated_iterator end() const
		{
			return fel::buffer<T>{(T*)ary, (T*)ary+sz}.end();
		}
	};
}