#pragma once

#include <common.h>
#include <cpu.h>
#include <cartridge.h>

namespace jmpr {

	class Bus {

		// To see...
		CPU* _cpu;
		Cartridge* _cart;

	public:

		Bus();

		void connectCPU(CPU* cpu) { _cpu = cpu; }
		void connectCartridge(Cartridge* cart) { _cart = cart; }

		u8 read(u16 address);
		void write(u16 address, u8 data);
	};
}