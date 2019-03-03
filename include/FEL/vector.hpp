#pragma once
#include "FEL/memory.hpp"
#include "FEL/buffer.hpp"
#include "FEL/algorithm/min_max.hpp"
#include "FEL/algorithm/move.hpp"
#include "FEL/algorithm/destroy.hpp"
namespace fel{

	template<typename T, typename allocator = fel::default_memory_allocator<T>>
	class vector
	{
		allocator alloc{};
		fel::buffer<T> backend;
		std::size_t sz = 0;
	public:
		using associated_iterator = typename fel::buffer<T>::associated_iterator;

		vector(std::size_t _sz = 12)
		: backend{(T*)alloc.allocate(_sz*sizeof(T)), _sz}
		{}

		vector(const vector& vec)
		: backend{(T*)alloc.allocate(vec.size()*sizeof(T)), vec.size()}
		{
			for(auto& elem : vec)
				push_back(elem);
		}

		vector(vector&& vec)
		{
			backend = vec.backend;
			vec.backend = buffer<T>{nullptr, 0};
		}

		vector& operator=(const vector& vec)
		{
			this->~vector();
			new(this) vector(vec);
			return *this;
		}

		vector& operator=(vector&& vec)
		{
			fel::swap(backend, vec.backend);
			fel::swap(sz, vec.sz);
		}

		~vector()
		{
			if(capacity())
			{
				auto to_clear = nameless_range<associated_iterator>{begin(),end()};
				destroy_range<T>(to_clear);
				alloc.deallocate(reinterpret_cast<typename allocator::pointer_type> (&*backend.begin()));
			}
		}

		constexpr associated_iterator begin() const
		{
			return backend.begin();
		}

		auto operator[] (std::size_t off)
		{
			return backend[off];
		}

		constexpr associated_iterator end() const
		{
			return backend.begin()+sz;
		}

		constexpr std::size_t size() const
		{
			return sz;
		}

		constexpr std::size_t capacity() const
		{
			return backend.size();
		}

		void reserve(std::size_t new_cap)
		{
			new_cap = fel::max(sz, new_cap);
			auto new_backend = fel::buffer<T>{alloc.allocate(new_cap), new_cap};
			fel::move_uninitialized<T>(backend, new_backend);
			alloc.deallocate(reinterpret_cast<typename allocator::pointer_type> (&*backend.begin()));
			backend = new_backend;
		}

		template<typename U>
		void push_back(const U& value)
		{
			if(sz+1 > capacity())
			{
				reserve(backend.size()*1.5f);
			}
			new(&*end()) T(value);
			++sz;
		}

		[[nodiscard]] typename fel::remove_reference<T>::type&& pop_back() 
		{
			return fel::forward<T>(operator[](--sz));
		}
	};
}