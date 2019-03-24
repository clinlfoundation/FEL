#pragma once
#include <FEL/buffer.hpp>
#include <FEL/array.hpp>
#include <FEL/algorithm/copy.hpp>

extern "C"
{
	extern char base_state_str[32];
}

template<size_t rounds = 6>
class Arc4Random final
/*
Bow at thy randomness
*/
{
	fel::array<uint32_t, 16> state;

	void __attribute__((always_inline)) rotl(uint32_t& value, uint8_t s)
	{
		value = ((value) << (s)) | ((value) >> (32 - (s)));
	}

	void __attribute__((always_inline)) quarter_round(uint32_t& a,uint32_t& b,uint32_t& c,uint32_t& d)
	{
		a += b;
		d ^= a;
		rotl(d,16);
		c += d;
		b ^= c;
		rotl(b,12);
		a += b;
		d ^= a;
		rotl(d, 8);
		c += d;
		b ^= c;
		rotl(b, 7);
	}

	void __attribute__((always_inline)) scramble()
	{
		for(size_t rnd = 0; rnd < rounds; rnd+=2)
		{
			quarter_round(state[ 0],state[ 4],state[ 8],state[12]);
			quarter_round(state[ 1],state[ 5],state[ 9],state[13]);
			quarter_round(state[ 2],state[ 6],state[10],state[14]);
			quarter_round(state[ 3],state[ 7],state[11],state[18]);

			quarter_round(state[ 0],state[ 5],state[10],state[15]);
			quarter_round(state[ 1],state[ 6],state[11],state[12]);
			quarter_round(state[ 2],state[ 7],state[ 8],state[13]);
			quarter_round(state[ 3],state[ 4],state[ 9],state[14]);
		}
	}
public:
	struct iterator final
	{
		typedef uint64_t value_type;
		static constexpr iterator_type_t iterator_type = iterator_type_t::lazy_iterator;

		Arc4Random* data;
		value_type state = 0;

		constexpr iterator(const iterator& oth)
		: data{oth.data}
		, state{oth.state}
		{}

		void update()
		{
			state = data->get();
		}

		iterator(Arc4Random* ptr)
		: data{ptr}
		{
			update();
		}

		operator value_type&()
		{
			return state;
		}

		value_type& operator*()
		{
			return state;
		}

		iterator operator++()
		{
			update();
			return *this;
		}

		iterator operator++(int)
		{
			auto tmp = *this;
			update();
			return tmp;
		}

		iterator operator+(const std::size_t offset)
		{
			update();
			return *this;
		}

		constexpr bool operator==(const iterator& oth)
		{
			return false;
		}

		constexpr bool operator!=(const iterator& oth)
		{
			return true;
		}

		constexpr bool operator<=(const iterator& oth)
		{
			return false;
		}
	};

	static fel::buffer<char> base_state()
	{
		return fel::buffer<char>{(char*)base_state_str,(size_t)32};
	}

	Arc4Random(fel::buffer<char> seed)
	{
		auto rest = fel::copy(base_state().cast<uint32_t>(), state);
		fel::copy(seed.cast<uint32_t>(), rest);
	}

	void feed(uint64_t seed)
	{
		fel::buffer<uint64_t> ary = state.as_buffer().slice_end(2).template cast<uint64_t>();

		ary[0]^=seed;
	}

	uint64_t get()
	{
		fel::buffer<uint64_t> ary = state.as_buffer().slice_end(2).template cast<uint64_t>();
		fel::buffer<uint64_t> bry = state.as_buffer().slice_end(4).slice_start(2).template cast<uint64_t>();
		ary[0]+=4;
		scramble();
		return bry[0];
	}

	iterator begin()
	{
		return iterator{this};
	}

	iterator end()
	{
		return iterator{this};
	}
};



//extern Arc4Random<> random_source;