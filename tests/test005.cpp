#include <FEL/arc4random.hpp>
#include <chrono>
#include <iostream>
#include <set>
#include <thread>
#include <random>


void test001()
{
	uint8_t devoid_seed[32] = {
		0x0F,
		0x1E,
		0x2D,
		0x3C,
		0x4B,
		0x5A,
		0x69,
		0x78,
		0x87,
		0x96,
		0xA5,
		0xB4,
		0xC3,
		0xD2,
		0xE1,
		0xF0,
		0x0F,
		0x1E,
		0x2D,
		0x3C,
		0x4B,
		0x5A,
		0x69,
		0x78,
		0x87,
		0x96,
		0xA5,
		0xB4,
		0xC3,
		0xD2,
		0xE1,
		0xF0
	};

	Arc4Random<> random_source{fel::buffer{(char*)devoid_seed, 32}};

	uint64_t v;

	const size_t loop=5000;
	auto begin = std::chrono::system_clock::now();

	for(size_t lo=0; lo<loop; lo++) {
		v^=random_source.get();
		v^=random_source.get();
		v^=random_source.get();
		v^=random_source.get();
		v^=random_source.get();

		v^=random_source.get();
		v^=random_source.get();
		v^=random_source.get();
		v^=random_source.get();
		v^=random_source.get();
	}

	auto end = std::chrono::system_clock::now();
	std::cout
	        << "PRNG Performance: 1 every "
	        << std::chrono::duration_cast< std::chrono::nanoseconds >(
	            end-begin
	        ).count()/loop/10
	        << "ns"
			<< "  ign"<<v
	        << std::endl;
}

void test002()
{
	uint8_t devoid_seed[32] = {
		0x0F,
		0x1E,
		0x2D,
		0x3C,
		0x4B,
		0x5A,
		0x69,
		0x78,
		0x87,
		0x96,
		0xA5,
		0xB4,
		0xC3,
		0xD2,
		0xE1,
		0xF0,
		0x0F,
		0x1E,
		0x2D,
		0x3C,
		0x4B,
		0x5A,
		0x69,
		0x78,
		0x87,
		0x96,
		0xA5,
		0xB4,
		0xC3,
		0xD2,
		0xE1,
		0xF0
	};

	Arc4Random<> random_source{fel::buffer{(char*)devoid_seed, 32}};

	uint64_t v;

	const size_t loop = 5000000;
	std::set<uint64_t> rec;

	std::thread friend_gen([&](){
	for(size_t i=0; i<loop*2; i++) {
		uint64_t data=random_source.get();
	}});

	for(size_t i=0; i<loop; i++) {
		uint64_t data=random_source.get();
		rec.insert(data);
	}

	int cumul[64]= {};

	for(auto val : rec) {
		for(auto b=0; b<64; b++) {
			cumul[b]+= ((1ULL<<b)&val)!=0;
		}
	}

	float stats[64]= {};
	int ok_bits=0;

	std::cout<<"Probability stats:\n";

	for(auto b=0; b<64; b++) {
		stats[b]= cumul[b]/(float)loop;
		std::cout<<"b"<<b<<":"<<stats[b]<<"\t";

		if(b>0 && b%8==0) {
			std::cout<<"\n";
		}

		ok_bits+=(stats[b]<0.58 && stats[b]>0.42);
	}

	std::cout<<"\nTotal of ok_bits:"<<ok_bits<<std::endl;
	friend_gen.join();
}

void test003()
{
	uint8_t devoid_seed[32] = {
		0x0F,
		0x1E,
		0x2D,
		0x3C,
		0x4B,
		0x5A,
		0x69,
		0x78,
		0x87,
		0x96,
		0xA5,
		0xB4,
		0xC3,
		0xD2,
		0xE1,
		0xF0,
		0x0F,
		0x1E,
		0x2D,
		0x3C,
		0x4B,
		0x5A,
		0x69,
		0x78,
		0x87,
		0x96,
		0xA5,
		0xB4,
		0xC3,
		0xD2,
		0xE1,
		0xF0
	};

	Arc4Random<> random_source{fel::buffer{(char*)devoid_seed, 32}};

	uint64_t v;
	
	const size_t loop = 5000000;
	std::set<uint64_t> rec;

	std::thread friend_gen([&](){
	for(size_t i=0; i<loop*2; i++) {
		uint64_t data=random_source.get();
	}});

	for(size_t i=0; i<loop; i++) {
		uint64_t data=random_source.get();
		rec.insert(data);
	}

	int cumul[64][64]= {};

	for(auto val : rec) {
		for(auto b=0; b<64; b++) {
			for(auto c=0; c<64; c++) {
				cumul[b][c]+= (((1ULL<<b)&val)!=0) && (((1ULL<<c)&val)!=0);
			}
		}
	}

	float stats[64][64]= {};
	int ok_bits=0;

	for(auto b=0; b<64; b++) {
		for(auto c=0; c<64; c++) {
			stats[b][c]= cumul[b][c]/(float)loop*2;
			ok_bits+=(stats[b][c]<0.58 && stats[b][c]>0.42);
		}
	}

	std::cout<<"Total of independantly ok_bits:"<<ok_bits<<" expects "<<64*63<<std::endl;

	friend_gen.join();
}

int main()
{
	test001();
	test002();
	test003();
}


















#include <FEL/compile_me.cpp>