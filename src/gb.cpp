#include <gb.h>

#include <thread>

namespace jmpr
{

	GameBoy *GameBoy::getInstance()
	{
		static GameBoy instance;
		return &instance;
	}

	GameBoy::GameBoy()
	{
		_bus = std::make_unique<Bus>();
		_cpu = std::make_unique<CPU>();
		_cart = std::make_unique<Cartridge>();
		_boot = std::make_unique<BootRom>();
		_ram = std::make_unique<RAM>();
		_apu = std::make_unique<APU>();
		_ppu = std::make_unique<PPU>(_cpu->getInterruptHandler());
		_joypad = std::make_unique<Joypad>(_cpu->getInterruptHandler());
		_timer = std::make_unique<Timer>(_apu.get(), _cpu->getInterruptHandler());
		_odma = std::make_unique<ObjectDMA>(_ppu->getOAM());
		_vdma = std::make_unique<VideoDMA>(_ppu->getVRAM());
		_io = std::make_unique<IO>(_joypad.get(), _timer.get(), _apu.get(), _ppu->getLCD(), _odma.get(), _vdma.get());
		_dbg = Debugger(_bus.get(), true);
		_ui = UI(_ppu.get(), _apu.get(), _boot.get());

		_running = false;
		_ticks = 0;
		_cgb_mode = false;
		_fps = 59.7f;
		_capped = true;

		// TODO add a ui component for this operation
		// _boot->load(std::string(DIRECTORY_PATH) + "/roms/boot/dmg.bin");
	}

	int GameBoy::runGameBoy()
	{
		_running = false;
		_ticks = 0;

		connectComponents();
		reboot();

		std::thread cpuThread([this]()
							  { cpuLoop(); });

		cpuThread.detach();

		uiLoop();

		_ui.cleanup();

		return 0;
	}

	void GameBoy::connectComponents()
	{
		_bus->connectCPU(_cpu.get());
		_bus->connectRAM(_ram.get());
		_bus->connectPPU(_ppu.get());
		_bus->connectIO(_io.get());
		_bus->connectCartridge(_cart.get());
		_bus->connectBoot(_boot.get());

		_cpu->connectBus(_bus.get());
		_cpu->connectVideoDMA(_vdma.get());
		_cpu->connectBoot(_boot.get());
		_ppu->connectVideoDMA(_vdma.get());
		_odma->connectBus(_bus.get());
		_vdma->connectBus(_bus.get());
		_cart->connectBoot(_boot.get());
	}

	void GameBoy::reboot()
	{
		_boot->reboot();
		_joypad->reboot();
		_odma->reboot();
		_vdma->reboot();
		_timer->reboot();
		_apu->reboot();
		_ppu->reboot();
		_cpu->reboot();
	}

	void GameBoy::cpuLoop()
	{
		while (_ui.isOpened())
		{
			if (_running)
			{
				_cpu->cycle();
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

		SDL_Delay(100);

		_cart->load(rom_file);

		if (!_cart->isValid())
			return false;

		std::cout << *_cart.get() << std::endl;
		_cgb_mode = _cart->isColor();

		reboot();

		_ticks = 0;

		return true;
	}

	bool GameBoy::setBootRom(const std::string &rom_file)
	{
		return _boot->load(rom_file);
	}

	void GameBoy::setVolume(float volume)
	{
		_apu->setVolume(volume);
	}

	void GameBoy::toggleAudioChannel(u8 id)
	{
		_apu->toggleChannel(id);
	}

	void GameBoy::pressButton(u8 button)
	{
		_joypad->pressInput(button);
	}

	void GameBoy::releaseButton(u8 button)
	{
		_joypad->releaseInput(button);
	}

	float GameBoy::getDesiredFrameLength()
	{
		return 1000.0f / GameBoy::getDesiredFPS();
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
				_timer->update();
				_ppu->update();
				_apu->update();

				_ticks++;
			}

			_odma->processDMA();
			_vdma->processDMA(_cpu->isHalted());
		}
	}
}
