#pragma once

#include <common.h>
#include <bus.h>
#include <cpu.h>
#include <cartridge.h>

namespace jmpr {

	class GameBoy {

		// Components
		Bus _bus;
		CPU _cpu;
		Cartridge _cart;

		// States
		bool _paused;
		bool _running;
		u64 _ticks;

	public:

		GameBoy();

		bool insertCartridge(const char* rom_file);

		void cycle(int cpuCycles);
	};

}