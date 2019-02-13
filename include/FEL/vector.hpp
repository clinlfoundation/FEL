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

		vector(std::size_t _sz = 12)
		: backend{(T*)alloc.allocate(_sz*sizeof(T)), _sz}
		{}

		~vector()
		{
			auto to_clear = nameless_range<associated_iterator>{begin(),end()};
			destroy_range<T>(to_clear);
			void* ptr = (void*)&*backend.begin();
			alloc.deallocate(ptr);
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
			auto new_backend = fel::buffer<T>{(T*)alloc.allocate(new_sz*sizeof(T)), new_sz};
			fel::move_uninitialized<T>(backend, new_backend);
			void* ptr = (void*)&*backend.begin();
			backend = new_backend;
			alloc.deallocate(ptr);
		}

		template<typename U>
		void push_back(const U& value)
		{
			if(sz+1>backend.size())
			{
				resize(backend.size()*1.5f);
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