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
		static Bus _bus;
		static CPU _cpu;
		static PPU _ppu;
		static APU _apu;
		static Cartridge _cart;
		static Joypad _joypad;
		static Timer _timer;
		static RAM _ram;
		static DMA _dma;
		static IO _io;
		static InterruptHandler _it_handler;

		static Debugger _dbg;

		// Interactive Components
		static UI _ui;

		// States
		static bool _running;
		static u64 _ticks;

	public:
		static int runGameBoy();

		static void connectComponents();
		static void reboot();

		static void cpuLoop();
		static void uiLoop();

		static bool insertCartridge(const std::string& rom_file);
		static void setVolume(float volume);
		static void toggleAudioChannel(u8 id);

		static void pressButton(u8 button);
		static void releaseButton(u8 button);

		static void cycle(u8 m_cycles);

		static void pause() { _running = false; }
		static void resume() { _running = true; }

		static void delay(u32 milliseconds) { SDL_Delay(milliseconds); }

		static u32 getCurrentTime() { return SDL_GetTicks(); }

		static void requestSaveHandling() { _cart.handleSaves(); }
	};
}