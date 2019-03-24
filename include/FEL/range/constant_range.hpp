#pragma once
#include <FEL/iterator.hpp>

namespace fel{
	template<typename T>
	class constant_range
	{
		T value;

		class iterator{
			constant_range* parent;

		public:
			operator T&(){return parent->value;}

			iterator operator++(){return *this;}
			iterator operator++(int){return *this;}
			iterator operator--(){return *this;}
			iterator operator--(int){return *this;}
			T operator*(){return parent->value;}

			bool operator==(const iterator& oth)
			{return parent->value==oth.parent->value;}
			bool operator!=(const iterator& oth)
			{return parent->value!=oth.parent->value;}

			typedef T value_type;
			static constexpr iterator_type_t iterator_type = iterator_type_t::lazy_iterator;

			iterator(constant_range* _parent)
			: parent{_parent}
			{}
		};
	public:
		using associated_iterator = iterator;

		constant_range(T& _value)
		: value{_value}
		{}

		constant_range(T _value)
		: value{_value}
		{}

		iterator begin()
		{
			return iterator{this};
		}
		iterator end()
		{
			return iterator{nullptr};
		}
	};
}