#pragma once

#include <common.h>
#include <bus.h>
#include <cpu.h>
#include <cartridge.h>
#include <ram.h>

#include <SDL/SDL.h>

namespace jmpr {

	class GameBoy {

		// Components
		static Bus _bus;
		static CPU _cpu;
		static Cartridge _cart;
		static Ram _ram;

		// States
		static bool _paused;
		static bool _running;
		static u64 _ticks;

	public:

		static int runGameBoy();

		static bool insertCartridge(const char* rom_file);

		static void cycle(int cpuCycles);

		static void delay(u32 milliseconds) { SDL_Delay(milliseconds); }
	};

}