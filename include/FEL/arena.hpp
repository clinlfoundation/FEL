#pragma once
#include "fel_config.hpp"
#include "FEL/buffer.hpp"
#include "FEL/type_traits"
#include <FEL/algorithm/tmp_manip.hpp>
/**
 *
 * Based of the code of buffer/arena.hpp from CLINL
 *  with permission of Ludovic Lagouardette <ludovic@nekoit.xyz>
 *  http://kernel.clinl.org/
 *
 */

template<typename page_allocator, size_t align = 1>
class arena{
	page_allocator allocator;
	fel::buffer<char> data;
	size_t last;
	size_t count;
public: 
	arena()
	:last(0)
	,count(0)
	,data(fel::buffer<char>(nullptr,nullptr))
	{}

	template<typename T = typename std::enable_if<fel::has_allocator_interface<page_allocator>::value,int>::type>
	arena(size_t sz)
	:last(0)
	,count(0)
	,data(nullptr,nullptr)
	{
		if(sz!=0)
		{
			auto v=allocator.allocate(sz);
			if(v!=nullptr)
			{
				data=fel::buffer<char>(reinterpret_cast<char*>(v),reinterpret_cast<char*>(v)+sz);
			}
		}
	}

	arena(char* pos,size_t sz)
	:last(0)
	,count(0)
	,data(pos,pos+sz)
	{
	}

	void* allocate(size_t sz)
	{
		[[maybe_unused]]
		size_t mod = 0;

		if constexpr (align != 1)
		{
			mod = align - ((intptr_t)data.begin())%align;
		}

		auto ret=data.begin()+last+mod;
		if(data.contains(ret))
		{
			count++;
			last+=sz+mod;
			return &*(ret);
		}
		else
		{
			return nullptr;
		}
	}

	bool deallocate(void* ptr)
	{
		if(data.contains((char*)ptr))
		{
			count--;
			if(count==0)
			{
				for(auto& i : data)
				{
					i=0;
				}
				last=0;
			}
			return true;
		}
		return false;
	}

	~arena()
	{
		if constexpr(fel::has_allocator_interface<page_allocator>::value)
		{
			allocator.deallocate(&data[0]);
		}
	}
};