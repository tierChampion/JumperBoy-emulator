// JumperBoy.cpp : Defines the entry point for the application.
//

#include <headers/jumper_boy.h>

using namespace jmpr;

int main(int argc, char* argv[])
{

	char const* filterPatterns[2] = { "*.gb", "*.gbc" };

	char const* selection = tinyfd_openFileDialog(
		"Select ROM",
		DIRECTORY_PATH,
		1,
		filterPatterns,
		NULL,
		0
	);

	Cartridge cart(selection);

	std::cout << cart << std::endl;

	return 0;
}
