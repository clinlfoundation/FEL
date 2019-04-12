#include "FEL/buffer.hpp"
#include "fel_config.hpp"

namespace fel{
	constexpr auto assert = fel_config::assert;

	namespace _impl{
		extern fel::buffer<char> assert_buffer;
	}
}