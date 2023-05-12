#pragma once

#include <common.h>

namespace jmpr {

	class Bus;
	class VRAM;

	class DMA {

		Bus* _bus;
		VRAM* _vram;

		u8 _dma;
		u8 _process_timer;

	public:

		DMA(VRAM* vram);

		void connectBus(Bus* bus) { _bus = bus; }

		void requestDMA(u8 source);
		void processDMA();
	};
}