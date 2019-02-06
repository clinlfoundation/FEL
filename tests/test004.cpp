#include <fstream>
#include <iostream>
#include <random>
#include <FEL/unordered_map.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <chrono>
int pass=0;
int fail=0;

int main(int argc, char** argv)
{
	fel::unordered_map<int, std::string> my_map;
	std::unordered_map<int, std::string> std_map;
	std::vector<int> ctrl;
	std::random_device rd;
    std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist{std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};

	for(int i = 0; i < 1200; i++)
	{
		int v = dist(gen);
		ctrl.push_back(v);
		my_map[v]=std::to_string(v);
		std_map[v]=std::to_string(v);
	}

	for(auto elem : ctrl)
	{
		if(std::to_string(elem) == my_map.get(elem))
			pass++;
		else
			fail++;
	}

	auto begin = std::chrono::high_resolution_clock::now();
	for(auto elem : ctrl)
	{
		my_map.get(elem);
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::cout<<"Access of fel::unordered_map: "<<std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count()/ctrl.size()<<"ns"<<std::endl;

	for(auto elem : ctrl)
	{
		if(std::to_string(elem) == my_map.get(elem))
			continue;
		else
			break;
	}

	begin = std::chrono::high_resolution_clock::now();
	for(auto elem : ctrl)
	{
		std_map.find(elem);
	}
	end = std::chrono::high_resolution_clock::now();
	std::cout<<"Access of std::unordered_map: "<<std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count()/ctrl.size()<<"ns"<<std::endl;

	/////////////////////////////////////////////

	std::ofstream{argv[fail?2:1]}<<"{\"pass\":"<<pass<<",\"fail\":"<<fail<<"}";
	return 0;
}