#include <fstream>
#include <iostream>
#include <FEL/buffer.hpp>
int pass=0;
int fail=0;

int main(int argc, char** argv)
{
	int ary[16];

	fel::buffer<int> my_buf{ary, ary+sizeof(ary)/sizeof(int)};
	{
		int count=1;
		for(auto& elem : my_buf)
			elem=count++;
	}

	/////////////////////////////////////////////

	if(my_buf.at(8).has_value())
		pass++;
	else
		fail++;

	/////////////////////////////////////////////

	if(!my_buf.at(32).has_value())
		pass++;
	else
		fail++;

	/////////////////////////////////////////////

	std::ofstream{argv[fail?2:1]}<<"{\"pass\":"<<pass<<",\"fail\":"<<fail<<"}";
	return 0;
}