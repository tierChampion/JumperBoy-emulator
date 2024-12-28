#pragma once

#include <common.h>

namespace jmpr {

	class Joypad;
	class Timer;
	class APU;
	class LCD;
	class ObjectDMA;
	class VideoDMA;
	class SpeedManager;

	// manage the io registers.
	class IO {

		Joypad* _joypad;
		u8 _serial_trans[2]; // cgb?
		Timer* _timer;
		APU* _apu;
		LCD* _lcd;
		ObjectDMA* _odma;
		VideoDMA* _vdma;
		SpeedManager* _speed;

	public:

		IO() {}
		IO(Joypad* pad, Timer* tim, APU* apu, LCD* lcd, ObjectDMA* odma, VideoDMA* vdma, SpeedManager* speed);

		u8 read(u16 address) const;
		void write(u16 address, u8 data);
	};
}