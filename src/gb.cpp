#include <gb.h>

namespace jmpr {

	Bus GameBoy::_bus = Bus();
	CPU GameBoy::_cpu = CPU();
	Cartridge GameBoy::_cart = Cartridge();
	Joypad GameBoy::_joypad = Joypad(_cpu.getInterruptHandler());
	Timer GameBoy::_timer = Timer(_cpu.getInterruptHandler());
	Ram GameBoy::_ram = Ram();
	IO GameBoy::_io = IO(&_joypad, &_timer);

	Debugger GameBoy::_dbg = Debugger(&_bus, true);

	UI GameBoy::_ui = UI(_joypad.getInputHandler());

	bool GameBoy::_paused = false;
	bool GameBoy::_running = false;
	u64 GameBoy::_ticks = 0;

	int GameBoy::runGameBoy() {

		_cpu.connectBus(&_bus);
		_bus.connectCPU(&_cpu);
		_bus.connectRam(&_ram);
		_bus.connectIO(&_io);

		_running = false;
		bool quitting = false;

		while (_ui.isOpened()) {

			if (_running) {

				_cpu.cycle();

				//_dbg.displayCycleSize(_ticks, _cpu.getOpcode());
				//_timer.displayStates();

				_dbg.update();
				_dbg.log();
			}

			_ui.handleEvents(_running);
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

	/**
	* Run all components other than the cpu for a given number of M-cycles.
	* One M-cycle is equivalent to 4 clock ticks or 4 T-states.
	*/
	void GameBoy::cycle(u8 m_cycles) {

		for (u16 t_state = 0; t_state < m_cycles * 4; t_state++) {

			_timer.update();
			// todo add the ppu and other stuff...

			_ticks++;
		}

		//if (_ticks == 256) {
		//	printf("stop...\n");
		//}
	}
}