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

		HaltBugStatus _bug_status; // apparently doesn't happen on gb color

	public:

		InterruptHandler() {}
		InterruptHandler(CPU* cpu);

		// todo with ei and di, takes time to apply
		void enableInterrupts();
		void disableInterrupts();

		u8 readInterruptEnabled() const { return _IE; }
		void writeInterruptEnabled(u8 data) { _IE = data; }

		u8 readInterruptFlags() const { return _IF; }

		void checkHaltBug(u16 currentPC);
		bool haltBugged(u16 currentPC);

		void requestInterrupt(const InterruptType type);
		void checkInterrupts();
	};
}