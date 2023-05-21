#include <ppu/dma.h>

#include <bus.h>
#include <ppu/vmem.h>

namespace jmpr {

	DMA::DMA(OAM* oam) {

		_bus = nullptr;
		_oam = oam;

		_dma = 0xFF;
		_process_timer = 0x00;
	}

	bool DMA::inProcess() {
		return _process_timer > 0;
	}

	void DMA::requestDMA(u8 source) {

		_dma = source; // MSB of the address to copie to OAM.
		_process_timer = 0xA0; // process is 160 cpu cycles long
	}

	void DMA::processDMA() {

		if (inProcess()) {

			u8 index = 0xA0 - _process_timer;
			u16 readAddress = (_dma * 0x100) + index;

			_oam->write(0xFE00 + index, _bus->read(readAddress), true);

			_process_timer--;
		}
	}

	u8 DMA::readDMA() const {
		return _dma;
	}
}