#include <debugging.h>

#include <bus.h>
#include <cpu.h>

namespace jmpr {


	Debugger::Debugger(Bus* bus) : _debug_log{ 0 } {

		_log_size = 0;

		_bus = bus;
	}

	void Debugger::update() {

		// Debug information is placed in the serial transfer
		if (_bus->read(0xFF02) == 0x81) {

			_debug_log[_log_size] = _bus->read(0xFF01);
			_log_size++;

			_bus->write(0xFF02, 0x00);
		}
	}

	void Debugger::log() {

		if (_debug_log[0]) {
			printf("DBG: %s\n", _debug_log);
		}
	}


	/* CPU INSTRUCTIONS TESTS */

	/**
	* Used to display all the possible cases of the DAA instruction.
	*/
	void CPU::testDAA() {

		u8 a_val = 0;
		u8 add_val = 0;

		for (a_val = 0; a_val <= 0x99; a_val = (a_val + 1) + 6 * ((a_val & 0xF) >= 0x9)) {
			for (add_val = 0; add_val <= 0x99; add_val = (add_val + 1) + 6 * ((add_val & 0xF) >= 0x9)) {
				// load a value into a
				_curr_instr = fromOpcode(0x3E); // load A <- d8
				_curr_fetch = a_val;
				execute();
				// add another value
				_curr_instr = fromOpcode(0xC6); // Add A <- d8
				_curr_fetch = add_val;

				execute();
				// daa
				_curr_instr = fromOpcode(0x27); // DAA
				execute();

				// display result
				printf("%02X + %02X = %02X (%X, %X, %X, %X)\n", a_val, add_val, _registers._A,
					zeroFlag(), negationFlag(), halfCarryFlag(), carryFlag());

				if (add_val + 1 == 0x9A) break;
			}

			if (a_val + 1 == 0x9A) break;
		}
	}

	/**
	* Used to display all the possible cases of the ADC instruction.
	*/
	void CPU::testADC() {

		u16 a_val = 0;
		u8 carry = 0;

		while (a_val < 0x100) {

			u16 add_val = 0;

			while (add_val < 0x100) {

				for (u8 i = 0; i < 2; i++) {

					setFlags(0, 0, 0, carry);

					// load a value into a
					_curr_instr = fromOpcode(0x3E); // load A <- d8
					_curr_fetch = a_val;
					execute();
					// add another value
					_curr_instr = fromOpcode(0xCE); // Adc A <- d8
					_curr_fetch = add_val;
					execute();

					// display result
					printf("%02X + %02X + <%X> = %02X (%X, %X, %X, %X)\n", a_val, add_val, carry, _registers._A,
						zeroFlag(), negationFlag(), halfCarryFlag(), carryFlag());

					carry ^= 1;

					_sleep(1);
				}

				add_val++;
			}

			a_val++;
		}

	}

	/**
	* Used to display all the possible cases of the SBC instruction.
	*/
	void CPU::testSBC() {

		u16 a_val = 0;
		u8 carry = 0;

		while (a_val < 0x100) {

			u16 sub_val = 0;

			while (sub_val < 0x100) {

				for (u8 i = 0; i < 2; i++) {

					setFlags(0, 0, 0, carry);

					// load a value into a
					_curr_instr = fromOpcode(0x3E); // load A <- d8
					_curr_fetch = a_val;
					execute();
					// add another value
					_curr_instr = fromOpcode(0xDE); // Adc A <- d8
					_curr_fetch = sub_val;
					execute();

					// display result
					printf("%02X - %02X - <%X> = %02X (%X, %X, %X, %X)\n",
						a_val, sub_val, carry, _registers._A, zeroFlag(), negationFlag(), halfCarryFlag(), carryFlag());

					carry ^= 1;

					_sleep(1);
				}

				sub_val++;
			}

			a_val++;
		}

	}

	/**
	* Used to display all the possible cases of the DAA instruction.
	*/
	void CPU::testSRA() {

		u16 a_val;

		for (a_val = 0; a_val <= 0xFF; a_val++) {
			// load a value into a
			_curr_instr = fromOpcode(0x3E); // load A <- d8
			_curr_fetch = a_val;
			execute();
			// add another value
			_curr_instr = fromOpcode(0xCB); // Add A <- d8
			_curr_fetch = 0x002F;
			execute();

			// display result
			printf("<SRA> %02X = %02X (%X, %X, %X, %X)\n", a_val, _registers._A,
				zeroFlag(), negationFlag(), halfCarryFlag(), carryFlag());
		}
	}
}