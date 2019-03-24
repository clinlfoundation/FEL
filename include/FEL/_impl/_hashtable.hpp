#include "fel_config.hpp"
#include "FEL/buffer.hpp"
#include "FEL/pair.hpp"
#include "FEL/memory.hpp"
#include "FEL/hash.hpp"
#include "FEL/algorithm/tmp_manip.hpp"
#include "FEL/algorithm/copy.hpp"
#include "FEL/range/display_ranges.hpp"
#include <type_traits>

namespace fel{

#if !defined(FEL_HASHMAP_NODE)
#define FEL_HASHMAP_NODE
template<class T>
struct node{
	uint64_t band = 0;
 	T* ptr= nullptr;
};
#endif //FEL_HASHMAP_NODE


#ifdef FEL_UNORDERED_MAP
	template<typename K, typename V, typename hash_type = fel::hash<K>, typename large_memory_allocator = default_memory_allocator<node<fel::pair<K,V>>>, typename node_memory_allocator = default_memory_allocator<fel::pair<K,V>>>
	class unordered_map{
#elif defined(FEL_UNORDERED_SET)
	template<typename V, typename hash_type = fel::hash<V>, typename large_memory_allocator = default_memory_allocator<node<V>>, typename node_memory_allocator = default_memory_allocator<V>>
	class unordered_set{
#endif 


		static std::size_t find_free_key_slot
		(
#ifdef FEL_UNORDERED_MAP
			const K key, 
			const buffer<node<fel::pair<K, V>>>& band
#elif defined(FEL_UNORDERED_SET)
			const V key, 
			const buffer<node<V>>& band
#endif 

		)
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
#ifdef FEL_UNORDERED_MAP
					if((*it).ptr->first == key)
#elif defined(FEL_UNORDERED_SET)
					if(*(*it).ptr == key)
#endif 
					{
						break;
					}
				}
				h=(h+1)%band.size();
				it = band.begin()+h;
			}
			return h;
		}

		constexpr static hash_type hash{};
		large_memory_allocator lma{};
		node_memory_allocator nma{};
		std::size_t _size;
#ifdef FEL_UNORDERED_MAP
		buffer<node<fel::pair<K, V>>> band;
#elif defined(FEL_UNORDERED_SET)
		buffer<node<V>> band;
#endif 
	public:

		class degrad_cursor{
#ifdef FEL_UNORDERED_MAP
			friend class unordered_map;
			K key;
			unordered_map* target;
#elif defined(FEL_UNORDERED_SET)
			friend class unordered_set;
			V key;
			unordered_set* target;
#endif 


			degrad_cursor(
#ifdef FEL_UNORDERED_MAP
				K _key, const unordered_map* _target
#elif defined(FEL_UNORDERED_SET)
				V _key, const unordered_set* _target
#endif 
			)
			: key{fel::move(_key)}
			, target{_target}
			{}
		public:
			void operator=(V& value)
			{
#ifdef FEL_UNORDERED_MAP
				target->insert(fel::move(key),value);
#elif defined(FEL_UNORDERED_SET)
				target->insert(value);
#endif 
			}
			void operator=(V&& value)
			{
#ifdef FEL_UNORDERED_MAP
				target->insert(fel::move(key),fel::move(value));
#elif defined(FEL_UNORDERED_SET)
				target->insert(fel::move(value));
#endif 
			}
			operator V&()
			{
				return target->get(key);
			}
		};

		struct iterator
		{
			static constexpr iterator_type_t iterator_type = iterator_type_t::non_contiguous_iterator;
#ifdef FEL_UNORDERED_MAP
			node<fel::pair<K, V>>* ptr;
#elif defined(FEL_UNORDERED_SET)
			node<V>* ptr;
#endif 

#ifdef FEL_UNORDERED_MAP
			constexpr iterator(node<fel::pair<K,V>>* orig)
#elif defined(FEL_UNORDERED_SET)
			constexpr iterator(node<V>* orig)
#endif 
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

			
			V& operator*()
			{
#ifdef FEL_UNORDERED_MAP
				return ptr->ptr->second;
#elif defined(FEL_UNORDERED_SET)
				return *(ptr->ptr);
#endif 
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

#ifdef FEL_UNORDERED_MAP
		unordered_map(std::size_t initial_capacity = 12, large_memory_allocator _lma = large_memory_allocator{}, node_memory_allocator _nma = node_memory_allocator{})
#elif defined(FEL_UNORDERED_SET)
		unordered_set(std::size_t initial_capacity = 12, large_memory_allocator _lma = large_memory_allocator{}, node_memory_allocator _nma = node_memory_allocator{})
#endif 
		: lma{_lma}
		, nma{_nma}
		, _size{0}
#ifdef FEL_UNORDERED_MAP
		, band((node<fel::pair<K, V>>*)lma.allocate(sizeof(node<fel::pair<K, V>>)*initial_capacity), initial_capacity)
		{
			new(&band[0]) node<fel::pair<K, V>>[initial_capacity];
		}
#elif defined(FEL_UNORDERED_SET)
		, band((node<V>*)lma.allocate(sizeof(node<V>)*initial_capacity), initial_capacity)
		{
			new(&band[0]) node<V>[initial_capacity];
		}
#endif 


#ifdef FEL_UNORDERED_MAP
		~unordered_map()
#elif defined(FEL_UNORDERED_SET)
		~unordered_set()
#endif 
		{
			for(auto& elem : band)
			{
				if(elem.ptr)
				{

#ifdef FEL_UNORDERED_MAP
					elem.ptr->~pair<K,V>();
#elif defined(FEL_UNORDERED_SET)
					elem.ptr->~V();
#endif 
					nma.deallocate(elem.ptr);
				}
			}
			lma.deallocate(&band[0]);
		}

		iterator begin() const
		{
			auto start = &*band.begin();
			while(start->ptr==nullptr && start!=&*band.begin()+band.size())
				++start;
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

		degrad_cursor operator[](
#ifdef FEL_UNORDERED_MAP
			K& key
#elif defined(FEL_UNORDERED_SET)
			V& key
#endif 
		) const
		{
			return degrad_cursor{
				key,
				this
			};
		}

		degrad_cursor operator[](
#ifdef FEL_UNORDERED_MAP
			K&& key
#elif defined(FEL_UNORDERED_SET)
			V&& key
#endif 
		) const
		{
			return degrad_cursor{
				.key = fel::move(key),
				.target = this
			};
		}

		void resize(std::size_t new_sz)
		{
#ifdef FEL_UNORDERED_MAP
			buffer<node<fel::pair<K, V>>> new_band((node<fel::pair<K, V>>*)lma.allocate(sizeof(node<fel::pair<K, V>>)*(new_sz+1)), new_sz);
			new(&new_band[0]) node<fel::pair<K, V>>[new_sz+1];
#elif defined(FEL_UNORDERED_SET)
			buffer<node<V>> new_band((node<V>*)lma.allocate(sizeof(node<V>)*(new_sz+1)), new_sz);
			new(&new_band[0]) node<V>[new_sz+1];
#endif 
			for(auto& elem : band)
			{
				if(elem.ptr!=nullptr)
				{
#ifdef FEL_UNORDERED_MAP
					auto offset = find_free_key_slot(elem.ptr->first, new_band);
#elif defined(FEL_UNORDERED_SET)
					auto offset = find_free_key_slot(*(elem.ptr), new_band);
#endif 
					auto& slot = new_band[offset];
					slot.band = elem.band;
					slot.ptr = elem.ptr;
				}
			}

			lma.deallocate(&band[0]);
			band = new_band;
		}

#ifdef FEL_UNORDERED_MAP
		template<typename tK, typename tV>
		void insert(tK key,tV value)
#elif defined(FEL_UNORDERED_SET)
		template<typename tV>
		void insert(tV value)
#endif 
		{
#ifdef FEL_UNORDERED_MAP
			fel::pair<K,V>* p = new(nma.allocate(sizeof(fel::pair<K,V>))) fel::pair<K,V>{fel::move(key),fel::move(value)};
			auto h = hash(p->first);
#elif defined(FEL_UNORDERED_SET)
			V* p = new(nma.allocate(sizeof(V))) V{fel::move(value)};
			auto h = hash(*p);
#endif 
			if(_size*3/2>=band.size())
				resize(band.size()*2);
#ifdef FEL_UNORDERED_MAP
			auto off = find_free_key_slot(p->first, band);
#elif defined(FEL_UNORDERED_SET)
			auto off = find_free_key_slot(*p, band);
#endif 
			auto& slot = band[off];
			if(slot.ptr == nullptr)
			{
				slot.band = h;
				slot.ptr = p;
				_size++;
				return;
			} else {
#ifdef FEL_UNORDERED_MAP
				slot.ptr->~pair<K,V>();
#elif defined(FEL_UNORDERED_SET)
				slot.ptr->~V();
#endif 
				nma.deallocate(slot.ptr);
				slot.ptr = p;
				return;
			}
		}

#ifdef FEL_UNORDERED_MAP
		void remove(K& key)
#elif defined(FEL_UNORDERED_SET)
		void remove(V& key)
#endif 
		{
#ifdef FEL_UNORDERED_MAP
			auto off = find_free_key_slot(key, band);
#elif defined(FEL_UNORDERED_SET)
			auto off = find_free_key_slot(key, band);
#endif
			auto& slot = band[off];
			if(slot.ptr!=nullptr)
			{
#ifdef FEL_UNORDERED_MAP
				slot.ptr->~pair<K,V>();
#elif defined(FEL_UNORDERED_SET)
				slot.ptr->~V();
#endif
				nma.deallocate(slot.ptr);
				slot.ptr = nullptr;
				resize(bucket_count());
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
#ifdef FEL_UNORDERED_MAP
			return slot.ptr->second;
#elif defined(FEL_UNORDERED_SET)
			return *slot.ptr;
#endif
		}

		template<typename tK>
		V& get_or(tK key, V& alt)
		{
			auto off = find_free_key_slot(key, band);
			auto& slot = band[off];
			
			if(slot.ptr == nullptr)
			{
				return alt;
			}
#ifdef FEL_UNORDERED_MAP
			return slot.ptr->second;
#elif defined(FEL_UNORDERED_SET)
			return *slot.ptr;
#endif
		}

		auto diagnose(buffer<char> i_buf) const
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
#ifdef FEL_UNORDERED_MAP
			auto text0 = "unordered_map statistics:\n\tband_size: ";
#elif defined(FEL_UNORDERED_SET)
			auto text0 = "unordered_set statistics:\n\tband_size: ";
#endif 
			auto btext0 = buffer{text0,text0+38};
			auto text1 = "\n\tsize: ";
			auto btext1 = buffer{text1,text1+8};
			auto text2 = "\n\tdisplacements: ";
			auto btext2 = buffer{text2,text2+17};
			auto text3 = "\n\tmax displacement: ";
			auto btext3 = buffer{text3,text3+20};
			stream = fel::copy(btext0, stream);

			stream = fel::copy(display_range{band.size()},stream);

			stream = fel::copy(btext1, stream);

			stream = fel::copy(display_range{size()},stream);

			stream = fel::copy(btext2, stream);
			
			stream = fel::copy(display_range{(double)displacements/(double)size()},stream);

			stream = fel::copy(btext3, stream);
			
			stream = fel::copy(display_range{max_displacement},stream);

			return stream;
		}
	};
}