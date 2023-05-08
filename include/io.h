#pragma once

#include <common.h>

namespace jmpr {

	class Joypad;
	class Timer;

	// manage the io registers.
	class IO {

		Joypad* _joypad;
		u8 _serial_trans[2];
		Timer* _timer;
		u8 _audio[0x16];
		u8 _wave_patts[0xF];
		u8 _lcd_stuff[0xC];
		u8 _vram_select;
		u8 _disable_bootrom;
		u8 _vram_dma[5];
		u8 _bg_obj_pallets[2];
		u8 _wram_select;

	public:

		IO(Joypad* pad, Timer* tim);

		u8 read(u16 address) const;
		void write(u16 address, u8 data);
	};
}