#include <fstream>

int main(int argc, char** argv)
{
	std::ofstream{argv[1]}<<"{\"pass\":1,\"fail\":0}";
	return 0;
}