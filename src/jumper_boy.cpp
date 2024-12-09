#include <gb.h>

int main(int argc, char* args[])
{
	std::cout << "ayo" << std::endl;
	return jmpr::GameBoy::getInstance()->runGameBoy();
}