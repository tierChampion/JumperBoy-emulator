#pragma once

#include <common.h>

namespace jmpr {

	class Bus;
	class OAM;
	class VRAM;

	class ObjectDMA {

		Bus* _bus;
		OAM* _oam;

		u8 _dma;
		u8 _process_timer;

	public:

		ObjectDMA() {}
		ObjectDMA(OAM* oam);

		void reboot();

		void connectBus(Bus* bus) { _bus = bus; }

		bool inProcess();
		void requestDMA(u8 source);
		void processDMA();

		u8 readDMA() const;
	};
	
	// todo create a new vram dma and adapt it
	class VideoDMA {

		Bus* _bus;
		VRAM* _vram;

		u16 _source;
		u16 _destination;
		u8 _mode;
		u16 _total_length;
		u16 _current_length;
		u16 _elapsed_length;

	public:

		VideoDMA() {}
		VideoDMA(VRAM* vram);

		void reboot();
		void connectBus(Bus* bus) { _bus = bus; }

		void write(u8 address, u8 data);

		void setStart(u8 start);
		bool inProcess();
		void requestDMA(u8 source);
		void processDMA(bool halted);
		void continueHBlankDMA();

		u8 readDMA() const;
	};
}