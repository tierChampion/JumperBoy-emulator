#pragma once

#include <common.h>

namespace jmpr {

	class Joypad;
	class Timer;
	class APU;
	class LCD;
	class ObjectDMA;
	class VideoDMA;

	// manage the io registers.
	class IO {

		Joypad* _joypad;
		u8 _serial_trans[2]; // cgb?
		Timer* _timer;
		APU* _apu;
		std::shared_ptr<LCD> _lcd;
		ObjectDMA* _odma;
		VideoDMA* _vdma;
		// TODO add a new class / datastructure to hold these registers
		// 		because they need to be accessed by the cpu for A. cycling and B. changing the stop
		u8 _vram_select; // cgb
		u8 _disable_bootrom;
		u8 _vram_dma[5]; // cgb
		u8 _bg_obj_pallets[2]; // cgb
		u8 _wram_select; // cgb
		u8 _key_1; // cgb

	public:

		IO() {}
		IO(Joypad* pad, Timer* tim, APU* apu, std::shared_ptr<LCD> lcd, ObjectDMA* odma, VideoDMA* vdma);

		u8 read(u16 address) const;
		void write(u16 address, u8 data);
	};
}