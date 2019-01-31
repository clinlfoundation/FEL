#include <fstream>
#include <iostream>
#include <FEL/buffer.hpp>
int pass=0;
int fail=0;

int main(int argc, char** argv)
{
	int ary[16];

	fel::buffer<int> my_buf{ary, ary+sizeof(ary)/sizeof(int)};
	
	if(my_buf.size()==16)
		pass++;
	else
		fail++;

	/////////////////////////////////////////////

	int count=1;
	for(auto& elem : my_buf)
		elem=count++;

	if(*(my_buf.end()-1)==16)
		pass++;
	else
		fail++;

	/////////////////////////////////////////////

	if(my_buf.contains(my_buf.begin()+8))
		pass++;
	else
		fail++;

	/////////////////////////////////////////////

	if(!my_buf.contains(my_buf.begin()-8))
		pass++;
	else
		fail++;

	/////////////////////////////////////////////

	if(!my_buf.contains(my_buf.begin()+32))
		pass++;
	else
		fail++;

	/////////////////////////////////////////////

	std::ofstream{argv[fail?2:1]}<<"{\"pass\":"<<pass<<",\"fail\":"<<fail<<"}";
	return 0;
}