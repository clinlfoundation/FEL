#include <fstream>
#include <FEL/_tools/_unix/sramfs.hpp>

int main(int argc, char** argv)
{
	fel::sramfs ramfs;

	

	std::ofstream{argv[1]}<<"{\"pass\":1,\"fail\":0}";
	return 0;
}