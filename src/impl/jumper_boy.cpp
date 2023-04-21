// JumperBoy.cpp : Defines the entry point for the application.
//

#include <headers/jumper_boy.h>

#include <headers/cartridge.h>

#include <Windows.h>

using namespace jmpr;

// TODO find a way to open a window to select the file

std::string openfilename(const char *filter = "GameBoyFiles (*.gb)\0*.gb", HWND owner = NULL) {
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = owner;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";
	ofn.lpstrInitialDir = "";

	std::string fileNameStr;
	if (GetOpenFileName(&ofn))
		fileNameStr = fileName;

	return fileNameStr;
}

int main(int argc, char* argv[])
{

	// For timers, define a certain amount of cycles to do.
	// In the GBA's case, there need to be 280 986 cpu cycles per frame
	// going at 59.73 Hz

	Cartridge cart((std::string(DIRECTORY_PATH) +
		std::string("/roms/Kirby's Dream Land (USA, Europe).gb")).c_str());

	std::cout << cart << std::endl;

	return 0;
}
