#pragma once
#include <FEL/endian.hpp>
#include <FEL/buffer.hpp>
#include <FEL/array.hpp>
#include <FEL/algorithm/copy.hpp>

extern "C"
{
	extern char base_state_str[32];
}

template<size_t rounds = 20>
class Chacha final
{
	fel::array<uint32_t, 16> state;

	static void __attribute__((always_inline)) rotl(uint32_t& value, const uint8_t& s)
	{
		value = ((value) << (s)) | ((value) >> (32 - (s)));
	}

	static void __attribute__((always_inline)) quarter_round(uint32_t& a,uint32_t& b,uint32_t& c,uint32_t& d)
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

	static void __attribute__((always_inline)) scramble(fel::array<uint32_t,16>& state)
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
	
	constexpr fel::buffer<uint32_t> nonce()
	{
		return state.slice_end(2);
	}
	
	constexpr void set_nonce(fel::buffer<uint32_t> v)
	{
		state.slice_end(2)[0]=v[0];
		state.slice_end(2)[1]=v[1];
	}
	
	constexpr fel::buffer<uint32_t> pos()
	{
		return state.slice_end(4).slice_begin(2);
	}
	
	void set_pos(const uint64_t& c)
	{
		auto n = pos().template cast<uint64_t>();
		n[0] = c;
		if constexpr (fel::endian::native != fel::endian::little)
		{
			swap_endian<uint64_t>(n[0]);
		}
	}
public:
	static fel::buffer<char> base_state()
	{
		return fel::buffer<char>{(char*)base_state_str,(size_t)32};
	}

	Chacha(fel::buffer<char> key, fel::buffer<uint32_t> x_nonce)
	{
		auto rest = fel::copy(base_state().cast<uint32_t>(), state);
		fel::copy(seed.cast<uint32_t>(), rest);
		fel::copy(x_nonce, nonce());
	}

	void encrypt(fel::array<uint32_t, 16>& data, uint64_t pos)
	{
		set_pos(pos);
		auto cpy = state;
		scramble(cpy);
		for(size_t i = 0; i<state.size(); ++i)
		{
			cpy[i]+=state[i];
			data[i]^cpy[i];
		}
	}
	
	void decrypt(fel::array<uint32_t, 16>& data, uint64_t pos)
	{
		encrypt(data,pos);
	}
};
