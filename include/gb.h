#pragma once

#include <common.h>
#include <bus.h>
#include <cpu/cpu.h>
#include <ppu/ppu.h>
#include <apu/apu.h>
#include <cartridge/cartridge.h>
#include <cartridge/boot.h>
#include <io/joypad.h>
#include <io/timer.h>
#include <ram.h>
#include <ppu/dma.h>
#include <io/io.h>
#include <io/speed_manager.h>
#include <cpu/debugging.h>
#include <ui/user_interface.h>

#include <SDL2/SDL.h>

namespace jmpr
{
	class GameBoy
	{
		// Internal Components
		std::unique_ptr<Bus> _bus;
		std::unique_ptr<CPU> _cpu;
		std::unique_ptr<PPU> _ppu;
		std::unique_ptr<APU> _apu;
		std::unique_ptr<Cartridge> _cart;
		std::unique_ptr<BootRom> _boot;
		std::unique_ptr<Joypad> _joypad;
		std::unique_ptr<Timer> _timer;
		std::unique_ptr<RAM> _ram;
		std::unique_ptr<ObjectDMA> _odma;
		std::unique_ptr<VideoDMA> _vdma;
		std::unique_ptr<IO> _io;
		std::unique_ptr<SpeedManager> _speed;

		Debugger _dbg;

		// Interactive Components
		UI _ui;

		// States
		bool _running;
		u64 _ticks;
		bool _cgb_mode;
		float _fps;
		bool _capped;

	public:
		static GameBoy *getInstance();
		int runGameBoy();

		void connectComponents();
		void reboot();

		void cpuLoop();
		void uiLoop();

		bool insertCartridge(const std::string &rom_file);
		bool setBootRom(const std::string &rom_file);
		void setVolume(float volume);
		void toggleAudioChannel(u8 id);

		void pressButton(u8 button);
		void releaseButton(u8 button);

		void cycle(u8 m_cycles);

		void pause() { _running = false; }
		void resume() { _running = true; }
		bool isRunning() { return _running; }

		bool isCGB() { return _cgb_mode; }

		float getDesiredFPS() { return _fps; }
		float getDesiredFrameLength();
		void setDesiredFPS(float newFps) { _fps = newFps; }
		bool isCapped() { return _capped; }
		void setCapped(bool newCapped) { _capped = newCapped; }

		void requestSaveHandling() { _cart->handleSaves(); }
		void setSaveFolder(const std::string &newFolder) { _cart->setSaveFolder(newFolder); }

	private:
		GameBoy();
	};
}
