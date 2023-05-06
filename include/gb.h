#pragma once

#include <common.h>
#include <bus.h>
#include <cpu.h>
#include <cartridge.h>
#include <joypad.h>
#include <ram.h>
#include <io.h>
#include <debugger.h>
#include <user_interface.h>

#include <SDL/SDL.h>

namespace jmpr {

	class GameBoy {

		// Internal Components
		static Bus _bus;
		static CPU _cpu;
		static Cartridge _cart;
		static Joypad _joypad;
		static Ram _ram;
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

		static bool insertCartridge(const char* rom_file);

		static void cycle(int m_cycles);

		static void delay(u32 milliseconds) { SDL_Delay(milliseconds); }
	};

}