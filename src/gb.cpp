#include <gb.h>

#include <thread>

namespace jmpr {

	Bus GameBoy::_bus = Bus();
	CPU GameBoy::_cpu = CPU();
	PPU GameBoy::_ppu = PPU();
	Cartridge GameBoy::_cart = Cartridge();
	Joypad GameBoy::_joypad = Joypad(_cpu.getInterruptHandler());
	Timer GameBoy::_timer = Timer(_cpu.getInterruptHandler());
	RAM GameBoy::_ram = RAM();
	DMA GameBoy::_dma = DMA(_ppu.getVRAM());
	IO GameBoy::_io = IO(&_joypad, &_timer, _ppu.getState(), &_dma);

	Debugger GameBoy::_dbg = Debugger(&_bus, true);

	UI GameBoy::_ui = UI(_joypad.getInputHandler());

	bool GameBoy::_paused = false;
	bool GameBoy::_running = false;
	u64 GameBoy::_ticks = 0;

	int GameBoy::runGameBoy() {

		_cpu.connectBus(&_bus);
		_bus.connectCPU(&_cpu);
		_bus.connectRAM(&_ram);
		_bus.connectVRAM(_ppu.getVRAM());
		_dma.connectBus(&_bus);
		_bus.connectIO(&_io);

		_running = false;
		bool quitting = false;

		std::thread cpuThread(&cpuLoop);
		cpuThread.detach();

		while (_ui.isOpened()) {

			_ui.displayTileData(&_bus);
			_ui.handleEvents(_running);

			delay(16); // about 50 fps
		}

		return 0;
	}

	void GameBoy::cpuLoop() {

		while (_ui.isOpened()) {
			if (_running) {

				_cpu.cycle();

				//_dbg.displayCycleSize(_ticks, _cpu.getOpcode());
				//_timer.displayStates();

				//_dbg.update();
				//_dbg.log();
			}
		}

		return;
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

		for (u8 cycle = 0; cycle < m_cycles; cycle++) {

			for (u8 t_state = 0; t_state < 4; t_state++) {

				_timer.update();

				_ticks++;
			}

			_dma.processDMA();
		}
	}
}