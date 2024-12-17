#include <cpu/interrupt.h>

#include <cpu/cpu.h>
#include <gb.h>

#include <unordered_map>


namespace jmpr {

	InterruptHandler::InterruptHandler() {

		_bug_status = HaltBugStatus();

		reboot();
	}

	InterruptHandler::InterruptHandler(CPU* cpu) {

		_cpu = cpu;
		_bug_status = HaltBugStatus();

		reboot();
	}

	/**
	* Reboot the Interrupt Handler and resets the registers to their initial values.
	*/
	void InterruptHandler::reboot() {

		_IME = true;
		_IME_schedule = true;
		_IE = 0x00;
		_IF = 0xE1;

		_bug_status._bugged = false;
		_bug_status._bug_address = 0x0000;
	}

	/**
	* Enable interrupts with the EI or RETI instruction.
	*/
	void InterruptHandler::enableInterrupts(bool instant) {

		if (instant) {
			_IME = true;
			_IME_schedule = 0x00;
		}
		else {
			_IME = false;
			_IME_schedule = 0x12;
		}
	}

	/**
	* Disable interrupts with the DI instruction.
	*/
	void InterruptHandler::disableInterrupts(bool instant) {

		if (instant) {
			_IME = false;
			_IME_schedule = 0x00;
		}
		else {
			_IME_schedule = 0x02;
		}
	}

	/**
	* Request an interrupt of a specific type.
	*/
	void InterruptHandler::requestInterrupt(const InterruptType type) {

		_IF = set(_IF, (u8)type);
	}

	/**
	* Check if the conditions are right for a HALT bug.
	*/
	void InterruptHandler::checkHaltBug(u16 currentPC) {

		if (((_IE & _IF) != 0) && !_IME) {
			_bug_status._bugged = true;
			_bug_status._bug_address = currentPC;
		}
		else {
			_bug_status._bugged = false;
		}
	}

	/**
	* Check if there was HALT bug.
	*/
	bool InterruptHandler::haltBugged(u16 currentPC) {

		bool bugged = (_bug_status._bugged) && (currentPC == _bug_status._bug_address);
		if (bugged) _bug_status._bugged = false;

		return bugged;
	}


	static inline const std::unordered_map<InterruptType, u16> INTERRUPT_LOCATIONS = {
		{InterruptType::VBLANK, 0x0040},
		{InterruptType::STAT, 0x0048},
		{InterruptType::TIMER, 0x0050},
		{InterruptType::SERIAL, 0x0058},
		{InterruptType::JOYPAD, 0x0060},
	};

	/**
	* Verify if there are pending interrupts.
	*/
	void InterruptHandler::checkInterrupts() {

		if (_IME_schedule > 0) {

			_IME_schedule--;

			if (loNibble(_IME_schedule) == 0) {

				_IME = hiNibble(_IME_schedule);
				_IME_schedule = 0x00;
			}
		}

		for (u8 interrupt = 0; interrupt < 5; interrupt++) {

			if (_IE & _IF & (1 << interrupt)) {

				InterruptType type = (InterruptType)interrupt;

				u16 interruptAddress = INTERRUPT_LOCATIONS.at(type);
				_cpu->executeInterrupt(_IME, type, interruptAddress);

				if (_IME) {
					_IME = false;
					_IF = reset(_IF, interrupt);
				}

				break;
			}
		}
	}

	/**
	* Have the CPU execute an interrupt. Similar to a call of the interrupt function.
	*/
	void CPU::executeInterrupt(bool enabled, InterruptType type, u16 location) {

		if (_halted)
			GameBoy::getInstance()->cycle(1);

		_halted = false;

		if (enabled) {

			GameBoy::getInstance()->cycle(2);

			pushStack8(hiByte(_PC));
			GameBoy::getInstance()->cycle(1);
			pushStack8(loByte(_PC));
			GameBoy::getInstance()->cycle(1);

			_PC = location;
			GameBoy::getInstance()->cycle(1);
		}

		if (type == InterruptType::JOYPAD)
			_stopped = false;
	}
}