#pragma once

#include <common.h>
#include <bus.h>
#include <cpu/cpu.h>
#include <ppu/ppu.h>
#include <cartridge/cartridge.h>
#include <io/joypad.h>
#include <io/timer.h>
#include <ram.h>
#include <ppu/dma.h>
#include <io/io.h>
#include <cpu/debugging.h>
#include <ui/user_interface.h>

#include <SDL/SDL.h>

namespace jmpr {

	class GameBoy {

		// Internal Components
		static Bus _bus;
		static CPU _cpu;
		static PPU _ppu;
		static Cartridge _cart;
		static Joypad _joypad;
		static Timer _timer;
		static RAM _ram;
		static DMA _dma;
		static IO _io;

		static Debugger _dbg;

		// Interactive Components
		static UI _ui;

		// States
		static bool _paused;
		static bool _running;
		static u64 _ticks;

	public:

		static int runGameBoy();

		static void cpuLoop();
		static void uiLoop();

		static bool insertCartridge(const char* rom_file);

		static void cycle(u8 m_cycles);

		static void delay(u32 milliseconds) { SDL_Delay(milliseconds); }

		static u32 getCurrentTime() { return SDL_GetTicks(); }
	};

}