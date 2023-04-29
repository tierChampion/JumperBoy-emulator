#include <gb.h>

#include <tinyfiledialogs.h>

namespace jmpr {

	Bus GameBoy::_bus = Bus();
	CPU GameBoy::_cpu = CPU();
	Cartridge GameBoy::_cart = Cartridge();
	Ram GameBoy::_ram = Ram();

	bool GameBoy::_paused = false;
	bool GameBoy::_running = false;
	u64 GameBoy::_ticks = 0;


	static const char* filterPatterns[2] = { "*.gb", "*.gbc" };

	int GameBoy::runGameBoy() {

		_cpu.connectBus(&_bus);
		_bus.connectCPU(&_cpu);
		_bus.connectRam(&_ram);

		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
			exit(-10);
		}

		const char* selection;

		// TODO
		SDL_Window* window = SDL_CreateWindow("gb", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 400, 0);

		_running = false;
		bool quitting = false;

		SDL_Event event;

		u8 test = 10;

		u8 val = 200;

		test += val;

		printf("%02X (%i)\n", test, val);

		while (!quitting) {

			if (_running) {
				_cpu.cycle();

				_ticks++;
			}

			while (SDL_PollEvent(&event)) {
				switch (event.type) {
					// X button on the screen
				case SDL_QUIT: quitting = true; break;

				case SDL_KEYDOWN:
					_cpu.cycle();
					_ticks++;
					break;

					// Run commands to create actions.
				case SDL_MOUSEBUTTONDOWN:

					const char* message = "Enter a command: open_rom, set_controls";

					// Input a command to process afterwards.
					const char* input = tinyfd_inputBox("Input command", message, "");

					if (input) {
						selection = tinyfd_openFileDialog(
							"Select ROM",
							DIRECTORY_PATH,
							1,
							filterPatterns,
							NULL,
							0
						);


						insertCartridge(selection);
						std::cout << _cart << std::endl;
					}

					break;
				}
			}

			delay(100);
		}

		SDL_DestroyWindow(window);
		SDL_Quit();

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