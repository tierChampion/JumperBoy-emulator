#include <ppu/dma.h>

#include <bus.h>
#include <ppu/vram.h>

namespace jmpr {

	DMA::DMA(VRAM* vram) {

		_bus = nullptr;
		_vram = vram;

		_dma = 0xFF;
		_process_timer = 0;
	}

	void DMA::requestDMA(u8 source) {

		_dma = source;
		_process_timer = 160; // process is 160 cpu cycles long
	}

	void DMA::processDMA() {

		if (_process_timer > 0) {

			u8 index = 160 - _process_timer;
			u16 readAddress = (_dma * 0x100) + index;

			_vram->writeOAM(0xFE00 + index, _bus->read(readAddress));

			_process_timer--;
		}
	}
}