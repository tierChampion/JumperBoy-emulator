#pragma once

#include <common.h>

namespace jmpr {

	class CPU;
	class Cartridge;
	class Ram;
	class IO;

	class Bus {

		CPU* _cpu;
		Cartridge* _cart;
		Ram* _ram;
		IO* _io;

	public:

		Bus();

		void connectCPU(CPU* cpu) { _cpu = cpu; }
		void connectCartridge(Cartridge* cart) { _cart = cart; }
		void connectRam(Ram* ram) { _ram = ram; }
		void connectIO(IO* io) { _io = io; }

		u8 read(u16 address);
		void write(u16 address, u8 data);
		void write16(u16 address, u16 data);
	};
}