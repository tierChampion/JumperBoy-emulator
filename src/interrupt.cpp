#include <interrupt.h>

#include <cpu.h>
#include <gb.h>

#include <unordered_map>


namespace jmpr {

	InterruptHandler::InterruptHandler(CPU* cpu) {
		_cpu = cpu;

		_IME = true;
		_IE = 0xFF;
		_IF = 0x00;

		_bug_status = HaltBugStatus();
	}

	void InterruptHandler::enableInterrupts() {
		_IME = true;
	}

	void InterruptHandler::disableInterrupts() {
		_IME = false;
	}

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

	void InterruptHandler::checkInterrupts() {

		for (u8 interrupt = 0; interrupt < 5; interrupt++) {

			if (_IE & _IF & (1 << interrupt)) {

				u16 interruptAddress = INTERRUPT_LOCATIONS.at((InterruptType)interrupt);
				_cpu->executeInterrupt(_IME, interruptAddress);

				if (_IME) {
					_IME = false;
					_IF = reset(_IF, interrupt);
				}

				break;
			}
		}
	}

	void CPU::executeInterrupt(bool enabled, u16 location) {

		_halted = false;
		GameBoy::cycle(1);

		if (enabled) {

			GameBoy::cycle(1);

			pushStack8(hiByte(_PC));
			GameBoy::cycle(1);
			pushStack8(loByte(_PC));
			GameBoy::cycle(1);

			_PC = location;
			GameBoy::cycle(1);
		}
	}
}