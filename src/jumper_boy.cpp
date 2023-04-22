// JumperBoy.cpp : Defines the entry point for the application.
//

#include <common.h>
#include <SDL/SDL.h>
#include <gb.h>
#include <tinyfiledialogs.h>

using namespace jmpr;

int main(int argc, char* argv[])
{

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {

		std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
		exit(1);
	}

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
