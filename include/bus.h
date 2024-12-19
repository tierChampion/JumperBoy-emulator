#pragma once

#include <common.h>

namespace jmpr {

	class CPU;
	class Cartridge;
	class BootRom;
	class RAM;
	class PPU;
	class IO;

	class Bus {

		CPU* _cpu;
		Cartridge* _cart;
		BootRom* _boot;
		RAM* _ram;
		PPU* _ppu;
		IO* _io;

	public:

		Bus();

		void connectCPU(CPU* cpu) { _cpu = cpu; }
		void connectCartridge(Cartridge* cart) { _cart = cart; }
		void connectBoot(BootRom* boot) { _boot = boot; }
		void connectRAM(RAM* ram) { _ram = ram; }
		void connectPPU(PPU* ppu) { _ppu = ppu; }
		void connectIO(IO* io) { _io = io; }

		u8 read(u16 address);
		void write(u16 address, u8 data);
	};
}