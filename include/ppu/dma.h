#pragma once

#include <common.h>

namespace jmpr {

	class Bus;
	class OAM;

	class DMA {

		Bus* _bus;
		OAM* _oam;

		u8 _dma;
		u8 _process_timer;

	public:

		DMA(OAM* oam);

		void connectBus(Bus* bus) { _bus = bus; }

		bool inProcess();
		void requestDMA(u8 source);
		void processDMA();
	};
}