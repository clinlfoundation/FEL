#pragma once
#include "fel_config.hpp"
#include "FEL/buffer.hpp"
#include "FEL/pair.hpp"
#include "FEL/algorithm/tmp_manip.hpp"
#include <type_traits>

namespace fel{
	template<typename T>
	uint64_t hash(const T& value)
	{
		if constexpr(value.begin && value.end)
		{
			uint64_t cumul = 0;
			for(auto elem : value)
				cumul += hash(elem);
			return cumul;
		}
		else if constexpr(std::is_floating_point<T>::value)
			return (value+1/value)*32771.0L; // closest prime to 32768 (source WolframAlpha)
		else if constexpr(std::is_integral<T>::value)
			return value;
	}


	

	template<typename K, typename V, typename I = std::enable_if_t<fel_config::memory_module::is_ok,int>>
	class unordered_map{
		struct node{
			uint64_t band = 0;
			fel::pair<K,V>* ptr = nullptr;
		};

		class degrad_cursor{
			friend class unordered_map;

			K key;
			unordered_map& target;
		public:
			void operator=(V value)
			{
				target->insert(std::move(key),fel::move(value));
			}
			void operator=(V&& value)
			{
				target->insert(fel::move(key),fel::move(value));
			}
			operator V&()
			{
				return target->get(key);
			}
		};

		static std::size_t find_free_key_slot(const K key, const buffer<node>& band)
		{
			auto oh = hash(key);
			auto h = oh%band.size();
			auto it = band.begin()+h;
			while(true)
			{
				if(it->ptr == nullptr)
				{
					break;
				}
				else if(it->band == oh)
				{
					if(it->ptr->first == key)
						break;
				}
				h=(h+1)%band.size();
				it = band.begin()+h;
			}
			return h;
		}

		std::size_t size;
		buffer<node> band;
	public:
		unordered_map(std::size_t initial_size = 12)
		: band((node*)fel_config::memory_module::memory_allocator(sizeof(node)*initial_size), initial_size)
		{
			new(&band[0]) node[initial_size];
		}

		degrad_cursor operator[] (K& key)
		{
			return degrad_cursor{
				.key = key,
				.target = this
			};
		}

		degrad_cursor operator[] (K&& key)
		{
			return degrad_cursor{
				.key = fel::move(key),
				.target = this
			};
		}

		void resize(std::size_t new_sz)
		{
			buffer<node> new_band((node*)fel_config::memory_module::memory_allocator(sizeof(node)*new_sz), new_sz);
			new(&new_band[0]) node[new_sz];

			for(auto elem : band)
			{
				if(elem->ptr!=nullptr)
				{
					auto offset = find_free_key_slot(elem->ptr->first, new_band);
					auto& slot = new_band[offset];
					slot.band = hash(elem->ptr->first);
					slot.ptr = elem->ptr;
				}
			}

			fel_config::memory_module::memory_deallocator(&band[0]);
			band = new_band;
		}

		template<typename tK, typename tV>
		void insert(tK key,tV value)
		{
			fel::pair<K,V>* p = new(fel_config::memory_module::memory_allocator(sizeof(fel::pair<K,V>))) fel::pair{fel::move(key),fel::move(value)};
			auto h = hash(p->first);
			if(size*3/2>=band.size())
				resize(band.size()*2);
			auto off = find_free_key_slot(p->first, band);
			auto& slot = band[off];
			if(slot.ptr == nullptr)
			{
				slot.band = h;
				slot.ptr = p;
				size++;
				return;
			} else {
				slot.ptr->~pair<K,V>();
				fel_config::memory_module::memory_deallocator(slot.ptr);
				slot.ptr = p;
				return;
			}

		}

		template<typename tK>
		typename fel::either<
			fel_config::has_exceptions,
				V&,
				fel::optional<V&>
		>::type get(tK key)
		{
			auto off = find_free_key_slot(key, band);
			auto& slot = band[off];
			
			if(slot.ptr == nullptr)
			{
				if constexpr (fel_config::has_exceptions)
				{
					throw bad_hashmap_access{};
				} 
				else
				{
					return fel::nullopt;
				}
			}
			return slot.ptr->second;
		}
	};
}