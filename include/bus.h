#pragma once

#include <common.h>

namespace jmpr {

	class CPU;
	class Cartridge;
	class RAM;
	class VRAM;
	class DMA;
	class IO;

	class Bus {

		CPU* _cpu;
		Cartridge* _cart;
		RAM* _ram;
		VRAM* _vram;
		DMA* _dma;
		IO* _io;

	public:

		Bus();

		void connectCPU(CPU* cpu) { _cpu = cpu; }
		void connectCartridge(Cartridge* cart) { _cart = cart; }
		void connectRAM(RAM* ram) { _ram = ram; }
		void connectVRAM(VRAM* vram) { _vram = vram; }
		void connectDMA(DMA* dma) { _dma = dma; }
		void connectIO(IO* io) { _io = io; }

		u8 read(u16 address);
		void write(u16 address, u8 data);
	};
}