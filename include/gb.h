#pragma once

#include <common.h>
#include <bus.h>
#include <cpu/cpu.h>
#include <ppu/ppu.h>
#include <apu/apu.h>
#include <cartridge/cartridge.h>
#include <io/joypad.h>
#include <io/timer.h>
#include <ram.h>
#include <ppu/dma.h>
#include <io/io.h>
#include <cpu/debugging.h>
#include <ui/user_interface.h>

#include <SDL2/SDL.h>

namespace jmpr
{
	class GameBoy
	{
		// Internal Components
		Bus _bus;
		CPU _cpu;
		std::shared_ptr<PPU> _ppu;
		APU _apu;
		Cartridge _cart;
		Joypad _joypad;
		Timer _timer;
		RAM _ram;
		ObjectDMA _odma;
		VideoDMA _vdma;
		IO _io;
		InterruptHandler _it_handler;

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
		static GameBoy* getInstance();
		int runGameBoy();

		void connectComponents();
		void reboot();

		void cpuLoop();
		void uiLoop();

		bool insertCartridge(const std::string &rom_file);
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

		void delay(u32 milliseconds) { SDL_Delay(milliseconds); }

		u32 getCurrentTime() { return SDL_GetTicks(); }

		void requestSaveHandling() { _cart.handleSaves(); }

	private:
		GameBoy();
	};
}