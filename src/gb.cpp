#include <gb.h>

#include <thread>

namespace jmpr
{
	Bus GameBoy::_bus = Bus();
	CPU GameBoy::_cpu = CPU();
	PPU GameBoy::_ppu = PPU(_cpu.getInterruptHandler());
	APU GameBoy::_apu = APU();
	Cartridge GameBoy::_cart = Cartridge();
	Joypad GameBoy::_joypad = Joypad(_cpu.getInterruptHandler());
	Timer GameBoy::_timer = Timer(&_apu, _cpu.getInterruptHandler());
	RAM GameBoy::_ram = RAM();
	ObjectDMA GameBoy::_odma = ObjectDMA(_ppu.getOAM());
	VideoDMA GameBoy::_vdma = VideoDMA(_ppu.getVRAM());
	IO GameBoy::_io = IO(&_joypad, &_timer, &_apu, _ppu.getLCD(), &_odma, &_vdma);

	Debugger GameBoy::_dbg = Debugger(&_bus, true);

	UI GameBoy::_ui = UI(&_ppu, &_apu);

	bool GameBoy::_running = false;
	u64 GameBoy::_ticks = 0;
	bool GameBoy::_cgb_mode = false;
	float GameBoy::_fps = 59.7f;
	bool GameBoy::_capped = true;

	int GameBoy::runGameBoy()
	{
		_running = false;
		_ticks = 0;

		connectComponents();

		std::thread cpuThread(&cpuLoop);

		cpuThread.detach();

		uiLoop();

		_ui.cleanup();

		return 0;
	}

	void GameBoy::connectComponents()
	{
		_bus.connectCPU(&_cpu);
		_bus.connectRAM(&_ram);
		_bus.connectPPU(&_ppu);
		_bus.connectIO(&_io);

		_cpu.connectBus(&_bus);
		_cpu.connectVideoDMA(&_vdma);
		_ppu.connectVideoDMA(&_vdma);
		_odma.connectBus(&_bus);
		_vdma.connectBus(&_bus);
	}

	void GameBoy::reboot()
	{
		_cpu.reboot();
		_ppu.reboot();
		_apu.reboot();
		_joypad.reboot();
		_timer.reboot();
		_odma.reboot();
		_vdma.reboot();
	}

	void GameBoy::cpuLoop()
	{
		while (_ui.isOpened())
		{
			if (_running)
			{
				_cpu.cycle();
			}
			else
			{
				SDL_Delay(1000);
			}
		}
	}

	void GameBoy::uiLoop()
	{
		while (_ui.isOpened())
		{
			_ui.loop(_running);
		}
	}

	bool GameBoy::insertCartridge(const std::string &rom_file)
	{
		pause();

		delay(100);

		_cart = Cartridge(rom_file);

		if (!_cart.isValid())
			return false;

		_cgb_mode = _cart.isColor();

		reboot();
		_bus.connectCartridge(&_cart);

		_ticks = 0;

		return true;
	}

	void GameBoy::setVolume(float volume)
	{
		_apu.setVolume(volume);
	}

	void GameBoy::toggleAudioChannel(u8 id)
	{
		_apu.toggleChannel(id);
	}

	void GameBoy::pressButton(u8 button)
	{
		_joypad.pressInput(button);
	}

	void GameBoy::releaseButton(u8 button)
	{
		_joypad.releaseInput(button);
	}

	/**
	 * Run all components other than the cpu for a given number of M-cycles.
	 * One M-cycle is equivalent to 4 clock ticks or 4 T-states.
	 */
	void GameBoy::cycle(u8 m_cycles)
	{
		for (u8 cycle = 0; cycle < m_cycles; cycle++)
		{
			for (u8 t_state = 0; t_state < 4; t_state++)
			{
				_timer.update();
				_ppu.update();
				_apu.update();

				_ticks++;
			}

			_odma.processDMA();
			_vdma.processDMA();
		}
	}
}
