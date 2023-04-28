#pragma once

#include <common.h>
#include <cpu.h>
#include <cartridge.h>
#include <ram.h>

namespace jmpr {

	class Bus {

		CPU* _cpu;
		Cartridge* _cart;
		Ram* _ram;

	public:

		Bus();

		void connectCPU(CPU* cpu) { _cpu = cpu; }
		void connectCartridge(Cartridge* cart) { _cart = cart; }
		void connectRam(Ram* ram) { _ram = ram; }

		u8 read(u16 address);
		void write(u16 address, u8 data);
		void write16(u16 address, u16 data);
	};
}