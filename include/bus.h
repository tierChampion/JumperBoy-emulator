#pragma once

#include <common.h>
#include <cpu.h>
#include <cartridge.h>

namespace jmpr {

	class Bus {

		// To see...
		CPU* _cpu;
		Cartridge* _cartridge;

	public:

		Bus();

		void attachCPU(CPU* cpu);
		void attachCartridge(Cartridge* cart);

		u8 read(u16 address);
		void write(u16 address, u8 data);
	};
}