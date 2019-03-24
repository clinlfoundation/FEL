#pragma once
#include "FEL/memory.hpp"
#include "FEL/buffer.hpp"
#include "FEL/algorithm/min_max.hpp"
#include "FEL/algorithm/move.hpp"
#include "FEL/algorithm/destroy.hpp"
namespace fel{

	template<typename T, typename allocator = fel::default_memory_allocator<>>
	class vector
	{
		allocator alloc{};
		fel::buffer<T> backend;
		std::size_t sz = 0;
	public:
		using associated_iterator = typename fel::buffer<T>::associated_iterator;

		vector(std::size_t _sz = 12, allocator def = allocator{})
		: alloc{def}
		, backend{(T*)alloc.allocate(_sz*sizeof(T)), _sz}
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
				void* ptr = (void*)&*backend.begin();
				alloc.deallocate(ptr);
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

		void resize(std::size_t new_sz)
		{
			new_sz = fel::max(sz, new_sz);
			if(new_sz>capacity()) reserve(new_sz);
			for(size_t idx = sz; idx<new_sz; idx++)
				new(&backend[idx]) T{};
			sz = new_sz;
		}

		void reserve(std::size_t new_cap)
		{
			new_cap = fel::max(capacity(), new_cap);
			auto new_backend = fel::buffer<T>{(T*)alloc.allocate(new_cap*sizeof(T)), new_cap};
			fel::move_uninitialized<T>(backend, new_backend);
			void* ptr = (void*)&*backend.begin();
			backend = new_backend;
			alloc.deallocate(ptr);
		}

		template<typename U>
		void push_back(const U& value)
		{
			if(sz+1>capacity())
			{
				reserve(capacity()*1.5f);
			}
			new(&*end()) T(value);
			++sz;
		}

		typename fel::remove_reference<T>::type&& pop_back()
		{
			return fel::forward<T>(operator[](--sz));
		}
	};
}