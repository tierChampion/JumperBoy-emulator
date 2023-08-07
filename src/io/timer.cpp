#include <io/timer.h>

#include <apu/apu.h>
#include <cpu/interrupt.h>

#include <array>

namespace jmpr {

	Timer::Timer(APU* apu, InterruptHandler* interHandler) {

		// AB is the div and E6 is for sync with the internal div
		reboot();

		_apu = apu;
		_it_handler = interHandler;
	}

	void Timer::reboot() {

		_div = 0xABCC;
		_tima = 0x00;
		_tma = 0x00;
		_tac = 0xF8;

		_old_output = computeOutput();
		_tima_overflow = 0x00;
	}

	// Frequency of the clock depending on the TAC register.
	static const std::array<u16, 4> TAC_TABLE = {
		0x400, // 1024 cpu cycles
		0x10, // 16 cpu cycles
		0x40, // 64 cpu cycles
		0x100 // 256 cpu cycles
	};

	/**
	* Compute the output of the DIV-TAC multiplexer.
	*/
	u8 Timer::computeOutput() const {
		bool timerBit = (_div & (TAC_TABLE[_tac & 0b11] >> 1));
		return timerBit && bit(_tac, 2);
	}

	/**
	* Compute the output of the DIV-APU counter.
	*/
	u8 Timer::computeDivApuOutput() const {
		return (_div & 0x2000) > 0; // adapt to double speed (0x4000)
	}

	/**
	* Update the timer for 1 clock.
	*/
	void Timer::update() {

		_div++;

		TIMAOverflowRoutine();

		u8 new_output = computeOutput();
		u8 new_divapu = computeDivApuOutput();

		// Increment TIMA register when falling edge
		if ((_old_output == 1) && (new_output == 0)) {

			_tima++;

			// TIMA overflow detection
			if (_tima == 0x00) {

				if (!_tima_overflow) {

					_tima_overflow = 0x8;
				}
			}
		}

		_apu->update(_div_apu_output, new_divapu);

		_old_output = new_output;
		_div_apu_output = new_divapu;
	}

	/**
	* Routine for when an overflow of the TIMA register is detected.
	*/
	void Timer::TIMAOverflowRoutine() {

		// TIMA overflow routine (one m-cycle later than the overflow)
		if (_tima_overflow > 0) {

			_tima_overflow--;

			if (_tima_overflow == 4 && _tima == 0) {
				_tima = _tma;
				_it_handler->requestInterrupt(InterruptType::TIMER);
			}
		}

	}

	u8 Timer::read(u8 address) const {

		u8 val = 0;

		switch (address) {
		case 0x04: val = hiByte(_div); break;
		case 0x05: val = _tima; break;
		case 0x06: val = _tma; break;
		case 0x07: val = _tac; break;
		default: printf("Error: Invalid timer register read.\n");
		}

		return val;
	}

	void Timer::write(u8 address, u8 data) {

		switch (address) {
		case 0x04: {

			_div = 0;

			// TIMA can get increased in certain cases
			if (_old_output == 1) {
				_tima++;
			}

			// TODO: apu falling edge check
			_apu->update(_div_apu_output, 0);

			_old_output = 0;
			_div_apu_output = 0;

			break;
		}
		case 0x05: {

			// If TIMA is not being set to TMA
			if (!between(_tima_overflow, 4, 1)) {
				_tima = data;
			}
			break;
		}
		case 0x06: {

			_tma = data;

			// If TIMA is being set to TMA
			if (between(_tima_overflow, 4, 1)) {
				_tima = _tma;
			}

			break;
		}
		case 0x07: {

			_tac = (data & 0b111) | 0xF8;

			u8 new_output = computeOutput();

			if ((_old_output == 1) && (new_output == 0)) {
				_tima++;
			}

			_old_output = new_output;

			break;
		}
		default: printf("Error: Invalid timer register write.\n");
		}
	}

	void Timer::displayStates() const {
		printf("<Timer> => DIV: %02X, TIMA: %02X, TMA: %02X, TAC: %02X\n",
			hiByte(_div), _tima, _tma, _tac);
	}
}