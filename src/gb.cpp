#include <gb.h>

#include <tinyfiledialogs.h>
#include <SDL/SDL.h>

namespace jmpr {

	Bus GameBoy::_bus = Bus();
	CPU GameBoy::_cpu = CPU();
	Cartridge GameBoy::_cart = Cartridge();

	bool GameBoy::_paused = false;
	bool GameBoy::_running = false;
	u64 GameBoy::_ticks = 0;


	int GameBoy::runGameBoy() {

		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
			exit(-10);
		}

		const char* filterPatterns[2] = { "*.gb", "*.gbc" };

		const char* selection = tinyfd_openFileDialog(
			"Select ROM",
			DIRECTORY_PATH,
			1,
			filterPatterns,
			NULL,
			0
		);

		if (!insertCartridge(selection)) {
			std::cerr << "Couldn't open the given ROM." << std::endl;
			return -1;
		}

		_running = true;

		while (_running) {
			_cpu.cycle();

			_ticks++;
		}

		return 0;
	}

	bool GameBoy::insertCartridge(const char* rom_file) {
		_cart = Cartridge(rom_file);

		if (!_cart.isValid()) return false;

		_bus.connectCartridge(&_cart);
		return true;
	}

	void GameBoy::cycle(int cpuCycles) {

		// TODO...
		// Run the ppu and the timer / clock, etc...
		// Everything that is in parallel with the CPU
	}
}