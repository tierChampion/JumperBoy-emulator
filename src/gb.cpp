#include <gb.h>

#include <tinyfiledialogs.h>

namespace jmpr {

	Bus GameBoy::_bus = Bus();
	CPU GameBoy::_cpu = CPU();
	Cartridge GameBoy::_cart = Cartridge();
	Ram GameBoy::_ram = Ram();

	UI GameBoy::_ui = UI();

	bool GameBoy::_paused = false;
	bool GameBoy::_running = false;
	u64 GameBoy::_ticks = 0;


	static const char* filterPatterns[2] = { "*.gb", "*.gbc" };

	int GameBoy::runGameBoy() {

		_cpu.connectBus(&_bus);
		_bus.connectCPU(&_cpu);
		_bus.connectRam(&_ram);

		_running = false;
		bool quitting = false;

		while (_ui.isOpened()) {

			if (_running) {
				_cpu.cycle();

				_ticks++;
			}

			_ui.handleEvents();

			delay(100);
		}

		return 0;
	}

	bool GameBoy::insertCartridge(const char* rom_file) {
		_cart = Cartridge(rom_file);

		if (!_cart.isValid()) return false;

		_bus.connectCartridge(&_cart);

		_running = true;

		return true;
	}

	void GameBoy::cycle(int m_cycles) {

		// TODO...
		// Run the ppu and the timer / clock, etc...
		// Everything that is in parallel with the CPU
	}
}