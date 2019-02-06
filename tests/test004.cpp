#include <fstream>
#include <iostream>
#include <random>
#include <FEL/unordered_map.hpp>
#include <string>
int pass=0;
int fail=0;

int main(int argc, char** argv)
{
	fel::unordered_map<int, std::string> my_map;
	std::random_device rd;
    std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist{std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};

	for(int i = 0; i < 12000; i++)
	{
		int v = dist(gen);
		my_map[v]=std::to_string(v);
	}

	/////////////////////////////////////////////

	std::ofstream{argv[fail?2:1]}<<"{\"pass\":"<<pass<<",\"fail\":"<<fail<<"}";
	return 0;
}