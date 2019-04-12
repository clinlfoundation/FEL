#include "fel_config.hpp"
#include "FEL/buffer.hpp"
extern "C"{
	char assert_buffer_data_store[fel_config::assert_buffer_size] = {};
}
namespace fel{
	namespace _impl{
		fel::buffer<char> assert_buffer{assert_buffer_data_store, fel_config::assert_buffer_size};
	}
}