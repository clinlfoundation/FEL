#pragma once
#include <cstdint>

namespace fel{
	namespace _impl{
		constexpr uint8_t tidybit[2] = {0x01,0x00};
	}
	
	enum class endian : uint16_t
	{
#ifdef _WIN32
		little = 0,
		big = 1,
		native = little
#elseif defined(__BYTE_ORDER__)
		little = __ORDER_LITTLE_ENDIAN__,
		big = __ORDER_BIG_ENDIAN__,
		native = __BYTE_ORDER__
#else
		little = 0x0001,
		big = 0x0100,
		native = *(uint16_t*)_impl::tidybit;
#endif
	};
	
	template <typename T>
	void swap_endian(T& value)
	{
		union
		{
			T v;
			uint8_t bytes[sizeof(T)];
		} src, dest;
	
		src.v = value;
		
		for (size_t idx = 0;idx<sizeof(T);idx++)
		{
			dest.u8[idx] = source.u8[sizeof(T)-idx-1];
		}
		
		value = dest.v;
	}
}