#include <gb.h>

namespace jmpr {

	GameBoy::GameBoy() {

		_bus = Bus();
		_cpu = CPU();
		_cpu.connectBus(&_bus);

		_paused = false;
		_running = true;
		_ticks = 0;
	}

	bool GameBoy::insertCartridge(const char* rom_file) {
		_cart = Cartridge(rom_file);

		return _cart.isValid();
	}
}