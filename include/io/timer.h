#pragma once

#include <common.h>

namespace jmpr {

	class APU;
	class InterruptHandler;

	class Timer {

		u16 _div;
		u8 _tima;
		u8 _tma;
		u8 _tac;

		u8 _edge;
		u8 _div_apu_edge;
		u8 _tima_overflow;

		APU* _apu;
		InterruptHandler* _it_handler;

	public:

		Timer() {}
		Timer(APU* apu, InterruptHandler* interHandler);

		void reboot();

		void displayStates() const;
		void update();

		u8 read(u8 address) const;
		void write(u8 address, u8 data);

	private:

		u8 computeEdge() const;
		bool checkFallingEdge();
		bool checkDivApuEdge();
		u8 computeDivApuEdge() const;
		void TIMAOverflowRoutine();
	};
}