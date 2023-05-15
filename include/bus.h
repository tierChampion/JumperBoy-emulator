#pragma once

#include <common.h>

namespace jmpr {

	class CPU;
	class Cartridge;
	class RAM;
	class VRAM;
	class OAM;
	class IO;

	class Bus {

		CPU* _cpu;
		Cartridge* _cart;
		RAM* _ram;
		VRAM* _vram;
		OAM* _oam;
		IO* _io;

	public:

		Bus();

		void connectCPU(CPU* cpu) { _cpu = cpu; }
		void connectCartridge(Cartridge* cart) { _cart = cart; }
		void connectRAM(RAM* ram) { _ram = ram; }
		void connectVRAM(VRAM* vram) { _vram = vram; }
		void connectOAM(OAM* oam) { _oam = oam; }
		void connectIO(IO* io) { _io = io; }

		u8 read(u16 address);
		void write(u16 address, u8 data);
	};
}