#pragma once
#include <FEL/algorithm/tmp_manip.hpp>
#include <type_traits>
#include <new>
#include "fel_config.hpp"
#include "FEL/exception.hpp"
#include "FEL/memory.hpp"
#include "FEL/function.hpp"


namespace fel{

	template< typename Enable, typename ...T>
	class variant;

	template<typename ...T>
	class variant<typename std::enable_if<!fel_config::memory_module::is_ok && all_of_fixed_size<T...>::value,int>::type, T...>{
		std::size_t index = std::numeric_limits<std::size_t>::max();
		char buffer[max_size<T...>];
		fel::function<void(void*)> dtor = [](void*){};
	public:
		template<typename U, typename std::enable_if<list_contains_class<U,T...>::value,int>::type>
		constexpr variant(U& value)
		: index{r_index_of<U, T...>::value}
		{
			new(buffer) U(value);
			dtor = fel::function([](void* thing){((U*)thing)->~U();});
		}

		template<typename U, typename std::enable_if<list_contains_class<U,T...>::value,int>::type>
		constexpr variant(U&& value)
		: index{r_index_of<U, T...>::value}
		{
			new(buffer) U(std::move(value));
			dtor = fel::function([](void* thing){((U*)thing)->~U();});
		}



		template<typename U, typename std::enable_if<list_contains_class<U,T...>::value,int>::type>
		void operator=(U& value)
		{
			if(index != std::numeric_limits<std::size_t>::max())
			{
				dtor((void*)buffer);
			}
			index = r_index_of<U, T...>::value;
			new(buffer) U(value);
			dtor = fel::function([](void* thing){((U*)thing)->~U();});
		}

		template<typename U, typename std::enable_if<list_contains_class<U,T...>::value,int>::type>
		void operator=(U&& value)
		{
			if(index != std::numeric_limits<std::size_t>::max())
			{
				dtor((void*)buffer);
			}
			index = r_index_of<U, T...>::value;
			new(buffer) U(std::move(value));
			dtor = fel::function([](void* thing){((U*)thing)->~U();});
		}

		~variant()
		{
			if(index != std::numeric_limits<std::size_t>::max())
			{
				dtor((void*)buffer);
			}
		}

		template<typename U>
		constexpr U& value()
		{
			if constexpr (fel_config::has_exceptions)
			{
				if(r_index_of<U, T...>::value != index)
				{
					throw bad_variant_access<U>{};
				}
			}
			return *reinterpret_cast<U*>(buffer);
		}

		template<typename U>
		constexpr U& is_a()
		{
			if(r_index_of<U, T...>::value == index)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	};

	template<typename ...T>
	class variant<typename std::enable_if<fel_config::memory_module::is_ok,default_memory_allocator<>>::type, T...>{
		std::size_t index = std::numeric_limits<std::size_t>::max();
		void* ptr;
		fel::function<void(void*)> dtor = [](void*){};
	public:
		template<typename U, typename std::enable_if<list_contains_class<U,T...>::value,int>::type>
		constexpr variant(U& value)
		: index{r_index_of<U, T...>::value}
		{
			ptr = (void*)new(default_memory_allocator<>{}.allocate(sizeof(U))) U(value);
			dtor = fel::function([](void* thing){((U*)thing)->~U();});
		}

		template<typename U, typename std::enable_if<list_contains_class<U,T...>::value,int>::type>
		constexpr variant(U&& value)
		: index{r_index_of<U, T...>::value}
		{
			ptr = (void*)new(default_memory_allocator<>{}.allocate(sizeof(U))) U(std::move(value));
			dtor = fel::function([](void* thing){((U*)thing)->~U();});
		}

		template<typename U, typename std::enable_if<list_contains_class<U,T...>::value,int>::type>
		void operator=(U& value)
		{
			if(index != std::numeric_limits<std::size_t>::max())
			{
				dtor(ptr);
				default_memory_allocator<>{}.deallocate(ptr);
			}
			index = r_index_of<U, T...>::value;
			ptr = (void*)new(default_memory_allocator<>{}.allocate(sizeof(U))) U(value);
			dtor = fel::function([](void* thing){((U*)thing)->~U();});
		}

		template<typename U, typename std::enable_if<list_contains_class<U,T...>::value,int>::type>
		void operator=(U&& value)
		{
			if(index != std::numeric_limits<std::size_t>::max())
			{
				dtor(ptr);
				default_memory_allocator<>{}.deallocate(ptr);
			}
			index = r_index_of<U, T...>::value;
			ptr = (void*)new(default_memory_allocator<>{}.allocate(sizeof(U))) U(std::move(value));
			dtor = fel::function([](void* thing){((U*)thing)->~U();});
		}

		~variant()
		{
			if(index != std::numeric_limits<std::size_t>::max())
			{
				dtor(ptr);
				default_memory_allocator<>{}.deallocate(ptr);
			}
		}

		template<typename U>
		constexpr U& value()
		{
			if constexpr (fel_config::has_exceptions)
			{
				if(r_index_of<U, T...>::value != index)
				{
					throw bad_variant_access<U>{};
				}
			}
			return *reinterpret_cast<U*>(ptr);
		}

		template<typename U>
		constexpr U& is_a()
		{
			if(r_index_of<U, T...>::value == index)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	};
}