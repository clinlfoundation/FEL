#pragma once

template<typename T, typename range_in>
void destroy_range(range_in& range)
{
	auto in = range.begin();
	auto in_close = range.end();
	while(in != in_close)
	{
		(&*(in++))->~T();
	}
}