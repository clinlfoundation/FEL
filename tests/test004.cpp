#include <fstream>
#include <iostream>
#include <random>
#include <FEL/unordered_map.hpp>
#include <FEL/unordered_set.hpp>
#include <FEL/vector.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <chrono>
int pass=0;
int fail=0;

int main(int argc, char** argv)
{
	std::random_device rd;
    std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist{std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};
	
	for(int loop = 10; loop<1200000; loop*=2.4)
	{
		for(int tries = 0; tries < 10; tries++)
		{
			fel::unordered_map<int, std::string> my_map;
			fel::unordered_set<int> my_set;
			std::unordered_map<int, std::string> std_map;
			fel::vector<int> ctrl;
			for(int i = 0; i < loop; i++)
			{
				int v = dist(gen);
				ctrl.push_back(v);
				my_set.insert(v);
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
			std::cout<<ctrl.size();

			auto begin = std::chrono::high_resolution_clock::now();
			for(auto elem : ctrl)
			{
				my_map.get(elem);
			}
			auto end = std::chrono::high_resolution_clock::now();
			std::cout<<"\t"<<std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count()/ctrl.size();

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
				*(std_map.find(elem));
			}
			end = std::chrono::high_resolution_clock::now();
			std::cout<<"\t"<<std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count()/ctrl.size()<<std::endl;

			char buff[2048];
			fel::buffer<char> as_buff{buff, buff+2048};

			auto range = my_map.diagnose(as_buff);
			for(auto c : fel::nameless_range<typename fel::buffer<char>::associated_iterator>{as_buff.begin(), range.begin()})
			{
				std::cerr<<c;
			}
			std::cerr<<std::endl;
		}
	}
	/////////////////////////////////////////////


	std::ofstream{argv[fail?2:1]}<<"{\"pass\":"<<pass<<",\"fail\":"<<fail<<"}";
	return 0;
}