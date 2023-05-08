#pragma once

#include <common.h>

namespace jmpr {

	class InterruptHandler;

	class Timer {

		u16 _div;
		u8 _tima;
		u8 _tma;
		u8 _tac;

		u8 _old_output;
		u8 _tima_overflow;

		InterruptHandler* _inter_handler;

	public:

		Timer(InterruptHandler* interHandler);

		void update();

		u8 read(u8 address);
		void write(u8 address, u8 data);

	private:

		u8 computeOutput();
		void TIMAOverflowRoutine();
	};
}