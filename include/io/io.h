#pragma once

#include <common.h>

namespace jmpr {

	class Joypad;
	class Timer;
	class APU;
	class LCD;
	class DMA;

	// manage the io registers.
	class IO {

		Joypad* _joypad;
		u8 _serial_trans[2]; //
		Timer* _timer;
		APU* _apu;
		LCD* _lcd;
		DMA* _dma;
		u8 _vram_select; // cgb
		u8 _disable_bootrom;
		u8 _vram_dma[5]; // cgb
		u8 _bg_obj_pallets[2]; // cgb
		u8 _wram_select; // cgb

	public:

		IO() {}
		IO(Joypad* pad, Timer* tim, APU* apu, LCD* lcd, DMA* dma);

		u8 read(u16 address) const;
		void write(u16 address, u8 data);
	};
}