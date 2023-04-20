// JumperBoy.cpp : Defines the entry point for the application.
//

#include <headers/jumper_boy.h>

#include <headers/cartridge.h>

using namespace jmpr;

int main(int argc, char* argv[])
{

	// For timers, define a certain amount of cycles to do.
	// In the GBA's case, there need to be 280 986 cpu cycles per frame
	// going at 59.73 Hz

	// Read more on VBlank, HBlank, blah blah blah...

	Cartridge cart((std::string(DIRECTORY_PATH) + std::string("/roms/Tetris (World) (Rev 1).gb")).c_str());

	std::cout << cart << std::endl;

	return 0;
}
