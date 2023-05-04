#pragma once

#include <common.h>

namespace jmpr {

	enum class InterruptType {
		VBLANK,
		STAT,
		TIMER,
		SERIAL,
		JOYPAD
	};

	struct HaltBugStatus {

		bool _bugged;
		u16 _bug_address;
	};

	class CPU;

	class InterruptHandler {

		CPU* _cpu;

		bool _IME;
		u8 _IE;
		u8 _IF;

		HaltBugStatus _bug_status;

		// stored the interrupt vars
		// can call an interrupt (jump to an interrupt function in the cpu)

		// use a friend class to have access to vars and  functions of cpu ?

		// Procedure:
		// If IF, IE and IME are set,
		// reset IME to prevent other interrupts
		// push pc to stack
		// jump at the start of interrupt
		// reset the bit in IF 

	public:

		InterruptHandler() {}
		InterruptHandler(CPU* cpu);

		void enableInterrupts();
		void disableInterrupts();

		u8 readInterruptEnabled() const { return _IE; }
		void writeInterruptEnabled(u8 data) { _IE = data; }

		void checkHaltBug(u16 currentPC);
		bool haltBugged(u16 currentPC);

		void requestInterrupt(const InterruptType type);
		void checkInterrupts();
	};
}