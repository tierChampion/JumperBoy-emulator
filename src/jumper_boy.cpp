// Errors:
// -1: Cartridge opening fail
// -2: Cartridge content fail
// -3: Instruction fail
// -9: Saves fail
// -10: SDL fail

#include <gb.h>

using namespace jmpr;

int main(int argc, char* argv[])
{
	return GameBoy::runGameBoy();
}
