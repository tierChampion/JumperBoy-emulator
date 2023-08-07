#include <gb.h>

#include <thread>

namespace jmpr {

	/**
	* TODO in the future:
	* APU
	* CGB functionnalities ?
	* More MBCs ?
	* SGB functionnalities ?
	*/

	Bus GameBoy::_bus = Bus();
	CPU GameBoy::_cpu = CPU();
	PPU GameBoy::_ppu = PPU(_cpu.getInterruptHandler());
	APU GameBoy::_apu = APU();
	Cartridge GameBoy::_cart = Cartridge();
	Joypad GameBoy::_joypad = Joypad(_cpu.getInterruptHandler());
	Timer GameBoy::_timer = Timer(&_apu, _cpu.getInterruptHandler());
	RAM GameBoy::_ram = RAM();
	DMA GameBoy::_dma = DMA(_ppu.getOAM());
	IO GameBoy::_io = IO(&_joypad, &_timer, &_apu, _ppu.getLCD(), &_dma);

	Debugger GameBoy::_dbg = Debugger(&_bus, true);

	UI GameBoy::_ui = UI(&_ppu, _joypad.getInputHandler());

	bool GameBoy::_paused = false;
	bool GameBoy::_running = false;
	u64 GameBoy::_ticks = 0;

	int GameBoy::runGameBoy() {

		_paused = false;
		_running = false;
		_ticks = 0;

		connectComponents();
		_running = false;

		std::thread cpuThread(&cpuLoop);

		cpuThread.detach();
		uiLoop();

		return 0;
	}

	void GameBoy::connectComponents() {

		_bus.connectCPU(&_cpu);
		_bus.connectRAM(&_ram);
		_bus.connectPPU(&_ppu);
		_bus.connectIO(&_io);

		_cpu.connectBus(&_bus);
		_dma.connectBus(&_bus);
	}

	void GameBoy::reboot() {

		_cpu.reboot();
		_ppu.reboot();
		_apu.reboot();
		_joypad.reboot();
		_timer.reboot();
		_dma.reboot();
		// io
	}

	void GameBoy::cpuLoop() {

		while (_ui.isOpened()) {
			if (_running) {

				_cpu.cycle();
			}
		}
	}

	void GameBoy::uiLoop() {

		while (_ui.isOpened()) {

			_ui.render();
			_ui.handleEvents(_running);
		}
	}

	bool GameBoy::insertCartridge(const char* rom_file) {

		_running = false;

		reboot();

		_cart = Cartridge(rom_file);

		if (!_cart.isValid()) return false;

		_bus.connectCartridge(&_cart);

		_running = true;
		_ticks = 0;

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
				_ppu.update();

				_ticks++;
			}

			_dma.processDMA();
		}
	}
}