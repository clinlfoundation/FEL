#pragma once
#include "fel_config.hpp"
#include "FEL/buffer.hpp"
#include "FEL/pair.hpp"
#include "FEL/memory.hpp"
#include "FEL/algorithm/tmp_manip.hpp"
#include "FEL/algorithm/copy.hpp"
#include <type_traits>

namespace fel{
	template<typename T>
	uint64_t hash(const T& value)
	{
		if constexpr(has_range_interface<T>::value)
		{
			uint64_t cumul = 0;
			for(auto elem : value)
				cumul += hash(elem);
			return cumul;
		}
		else if constexpr(std::is_floating_point<T>::value)
		{
			if constexpr(sizeof(T)==2)
				return *(uint16_t*)&value;
			if constexpr(sizeof(T)==4)
				return *(uint32_t*)&value;
			if constexpr(sizeof(T)==8)
				return *(uint64_t*)&value;
		}
		else if constexpr(std::is_integral<T>::value)
			return value;
	}

	template<typename K, typename V, typename large_memory_allocator = default_memory_allocator<>, typename node_memory_allocator = default_memory_allocator<>>
	class unordered_map{
		struct node{
			uint64_t band = 0;
			fel::pair<K,V>* ptr = nullptr;
		};

		static std::size_t find_free_key_slot(const K key, const buffer<node>& band)
		{
			auto oh = hash(key);
			auto h = oh%band.size();
			auto it = band.begin()+h;
			while(true)
			{
				if((*it).ptr == nullptr)
				{
					break;
				}
				else if((*it).band == oh)
				{
					if((*it).ptr->first == key)
						break;
				}
				h=(h+1)%band.size();
				it = band.begin()+h;
			}
			return h;
		}

		large_memory_allocator lma{};
		node_memory_allocator nma{};
		std::size_t _size;
		buffer<node> band;
	public:

		class degrad_cursor{
			friend class unordered_map;

			K key;
			unordered_map* target;

			degrad_cursor(K& _key, unordered_map* _target)
			: key{_key}
			, target{_target}
			{}
		public:
			void operator=(V& value)
			{
				target->insert(fel::move(key),value);
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

		struct iterator
		{
			static constexpr iterator_type_t iterator_type = iterator_type_t::non_contiguous_iterator;
			node* ptr;

			constexpr iterator(node* orig)
			: ptr{orig}
			{}

			iterator operator++()
			{
				do
				{
					ptr++;
				}
				while(ptr->ptr!=nullptr);
				return iterator{ptr};
			}
			iterator operator++(int)
			{
				auto tmp = ptr;
				do
				{
					ptr++;
				}
				while(ptr->ptr!=nullptr);
				return iterator{ptr};
			}
			iterator operator--()
			{
				do
				{
					ptr--;
				}
				while(ptr->ptr!=nullptr);
				return iterator{ptr};
			}
			iterator operator--(int)
			{
				auto tmp = ptr;
				do
				{
					ptr--;
				}
				while(ptr->ptr!=nullptr);
				return iterator{ptr};
			}
			bool operator==(iterator& oth)
			{
				return ptr==oth.ptr;
			}
			bool operator!=(iterator& oth)
			{
				return ptr!=oth.ptr;
			}
		};

		using associated_iterator = iterator;

		unordered_map(std::size_t initial_capacity = 12, large_memory_allocator _lma = large_memory_allocator{}, node_memory_allocator _nma = node_memory_allocator{})
		: lma{_lma}
		, nma{_nma}
		, _size{0}
		, band((node*)lma.allocate(sizeof(node)*initial_capacity), initial_capacity)
		{
			new(&band[0]) node[initial_capacity];
		}

		~unordered_map()
		{
			for(auto& elem : band)
			{
				if(elem.ptr)
				{
					elem.ptr->~pair<K,V>();
					nma.deallocate(elem.ptr);
				}
			}
			lma.deallocate(&band[0]);
		}

		iterator begin() const
		{
			auto start = &*band.begin();
			if(start.ptr==nullptr)
				return ++start;
			return start;
		}

		iterator end() const
		{
			return &*band.end();
		}

		constexpr std::size_t size() const
		{
			return _size;
		}

		constexpr std::size_t bucket_count() const
		{
			return band.size();
		}

		degrad_cursor operator[] (K& key)
		{
			return degrad_cursor{
				key,
				this
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
			buffer<node> new_band((node*)lma.allocate(sizeof(node)*(new_sz+1)), new_sz);
			new(&new_band[0]) node[new_sz+1];

			for(auto& elem : band)
			{
				if(elem.ptr!=nullptr)
				{
					auto offset = find_free_key_slot(elem.ptr->first, new_band);
					auto& slot = new_band[offset];
					slot.band = elem.band;
					slot.ptr = elem.ptr;
				}
			}

			lma.deallocate(&band[0]);
			band = new_band;
		}

		template<typename tK, typename tV>
		void insert(tK key,tV value)
		{
			fel::pair<K,V>* p = new(nma.allocate(sizeof(fel::pair<K,V>))) fel::pair<K,V>{fel::move(key),fel::move(value)};
			auto h = hash(p->first);
			if(_size*3/2>=band.size())
				resize(band.size()*2);
			auto off = find_free_key_slot(p->first, band);
			auto& slot = band[off];
			if(slot.ptr == nullptr)
			{
				slot.band = h;
				slot.ptr = p;
				_size++;
				return;
			} else {
				slot.ptr->~pair<K,V>();
				nma.deallocate(slot.ptr);
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

		auto diagnose(buffer<char> i_buf)
		{
			auto stream = nameless_range<typename buffer<char>::associated_iterator>(i_buf.begin(), i_buf.end());
			size_t displacements = 0;
			size_t max_displacement = 0;
			size_t tmp = 0;
			for(size_t i = 0; i<band.size(); i++)
			{
				const auto& elem = *(band.begin()+i);
				if(elem.ptr != nullptr)
				{
					size_t normal = elem.band % band.size();
					size_t actual = i;
					size_t displacement = (normal < actual ? actual - normal : band.size() + actual - normal) % band.size();
					displacements += displacement;
					max_displacement = fel::max(max_displacement, displacement);
				}
			}
			auto text0 = "unordered_map statistics:\n\tband_size: ";
			auto btext0 = buffer{text0,text0+38};
			auto text1 = "\n\tsize: ";
			auto btext1 = buffer{text1,text1+8};
			auto text2 = "\n\tdisplacements: ";
			auto btext2 = buffer{text2,text2+17};
			auto text3 = "\n\tmax displacement: ";
			auto btext3 = buffer{text3,text3+20};
			stream = fel::copy(btext0, stream);

			stream = fel::copy(std::to_string(band.size()),stream);

			stream = fel::copy(btext1, stream);

			stream = fel::copy(std::to_string(size()),stream);

			stream = fel::copy(btext2, stream);
			
			stream = fel::copy(std::to_string((double)displacements/(double)size()),stream);

			stream = fel::copy(btext3, stream);
			
			stream = fel::copy(std::to_string(max_displacement),stream);

			return stream;
		}
	};
}