#pragma once
#include <cstddef>
#include <cstdint>
#include <FEL/optional.hpp>
#include <FEL/iterator.hpp>
#include <FEL/function.hpp>
#include <FEL/exception.hpp>
#include <FEL/algorithm/move.hpp>

namespace fel{

	template<typename T>
	class buffer final{
	public:
		struct buffer_iterator final
		{
			T* data;
			typedef T value_type;
			typedef std::size_t difference_type;
			static constexpr iterator_type_t iterator_type = iterator_type_t::contiguous_iterator;

			constexpr buffer_iterator(const buffer_iterator& oth)
			: data{oth.data}
			{}
 
			constexpr buffer_iterator(T* ptr)
			: data{ptr}
			{}

			constexpr operator T&()
			{
				return *data;
			}

			constexpr T& operator*(){
				return *data;
			}

			constexpr buffer_iterator operator++()
			{
				return buffer_iterator{++data};
			}

			constexpr buffer_iterator operator++(int)
			{
				return buffer_iterator{data++};
			}

			constexpr buffer_iterator operator--()
			{
				return buffer_iterator{--data};
			}

			constexpr buffer_iterator operator--(int)
			{
				return buffer_iterator{data--};
			}

			constexpr buffer_iterator operator+(const std::size_t offset)
			{
				return buffer_iterator{data+offset};
			}

			constexpr buffer_iterator operator+(const int offset)
			{
				return buffer_iterator{data+offset};
			}

			constexpr buffer_iterator operator-(const std::size_t offset)
			{
				return buffer_iterator{data-offset};
			}

			constexpr buffer_iterator operator-(const int offset)
			{
				return buffer_iterator{data-offset};
			}

			constexpr difference_type operator-(const buffer_iterator& oth) const
			{
				return (T*)data-(T*)oth.data;
			}

			constexpr bool operator==(const buffer_iterator& oth)
			{
				return data==oth.data;
			}

			constexpr bool operator!=(buffer_iterator& oth)
			{
				return data!=oth.data;
			}

			constexpr bool before_or_equal(const buffer_iterator& oth)
			{
				return reinterpret_cast<std::intptr_t>(data) <= reinterpret_cast<std::intptr_t>(oth.data);
			}

			constexpr bool operator<=(const buffer_iterator& oth)
			{
				return before_or_equal(oth);
			}
		};
	private:
		buffer_iterator begin_elem;
		buffer_iterator end_elem;
	public:
		using associated_iterator = buffer_iterator;

		constexpr buffer(T* beg_ptr, T* end_ptr)
		: begin_elem{beg_ptr}
		, end_elem{end_ptr}
		{}

		constexpr buffer(T* beg_ptr, std::size_t sz)
		: begin_elem{beg_ptr}
		, end_elem{beg_ptr+sz}
		{}

		constexpr typename buffer_iterator::difference_type size() const
		{
			return end_elem - begin_elem;
		}

		constexpr associated_iterator begin() const
		{
			return begin_elem;
		}

		constexpr associated_iterator end() const
		{
			return end_elem;
		}

		constexpr T& operator[](std::size_t offset)
		{
			return *(begin_elem+offset);
		}

		optional<buffer_iterator> at(std::size_t offset)
		{
			auto elem = begin()+offset;
			if(!contains(elem))
			{
				return nullopt;
			}
			return elem;
		}

		constexpr bool contains(buffer_iterator ptr)
		{
			return 
				ptr.data < end_elem.data
				&& ptr.data >= begin_elem.data;
		}

		template<typename U>
		buffer<U> cast()
		{
			if constexpr(sizeof(T)%sizeof(U) == 0)
			{
				return buffer<U>(reinterpret_cast<U*>(&*begin_elem), size()*(sizeof(T)/sizeof(U)));
			}
			else
			{
				if(size()*sizeof(T)/sizeof(U))
				{
					return buffer<U>(reinterpret_cast<U*>(&*begin_elem), size()*sizeof(T)/sizeof(U));
				}
				else if constexpr (fel_config::has_exceptions)
				{
					throw bad_buffer_cast<T, U>{};
				}
				else
				{
					return buffer<U>(reinterpret_cast<U*>(nullptr), 0);
				}
			}
		}

		buffer slice_start(size_t new_sz)
		{
			if(new_sz<=size())
			{
				return buffer{&*begin(), &*(begin()+new_sz)};
			}
			else
			{
				return buffer{(T*)nullptr,(size_t)0};
			}
		}

		buffer slice_end(size_t new_sz)
		{
			if(new_sz<=size())
			{
				return buffer{&*(end()-(1+new_sz)), &*end()};
			}
			else
			{
				return buffer{(T*)nullptr,(size_t)0};
			}
		}
	};

	template<typename T>
	class unique_buffer final{
		buffer<T> data;
		fel::function<void(void*)> deallocator;
	public:
		using associated_iterator = typename buffer<T>::associated_iterator;

		unique_buffer(T* beg_ptr, T* end_ptr, fel::function<void(void*)> deallocator_impl)
		: data{beg_ptr,end_ptr}
		, deallocator{deallocator_impl}
		{}

		unique_buffer(unique_buffer&) = delete;
		unique_buffer& operator=(unique_buffer&) = delete;

		unique_buffer(unique_buffer&& oth)
		: data{oth.data}
		, deallocator{oth.deallocator}
		{
			oth.data=buffer<T>{nullptr,nullptr};
		}

		void operator=(unique_buffer&& oth)
		{
			fel::swap(data, oth.data);
			fel::swap(deallocator, oth.deallocator);
		}

		~unique_buffer()
		{
			if(!data.contains(nullptr))
			{
				for(auto& elem : data)
				{
					elem.~T();
				}
				deallocator(&*data.begin());
			}
		}

		constexpr auto begin()
		{
			return data.begin();
		}

		constexpr auto end()
		{
			return data.end();
		}

		constexpr auto size()
		{
			return data.size();
		}

		constexpr T& operator[](std::size_t offset)
		{
			return data[offset];
		}

		constexpr auto at(std::size_t offset)
		{
			return data.at(offset);
		}

		constexpr bool contains(typename buffer<T>::buffer_iterator ptr)
		{
			return data.contains(ptr);
		}
	};
}