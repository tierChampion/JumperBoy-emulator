#pragma once

#include <common.h>

namespace jmpr {

	class Bus;

	/**
	* Class used for console display of the Blargg test roms.
	*/
	class Debugger {

		Bus* _bus;

		char _debug_log[1024];
		u16 _log_size;

		bool _constant;
		bool _modified;

		u64 _old_time;

	public:

		Debugger(Bus* bus, bool constant);

		void update();
		void log();

		void displayCycleSize(u64 currentTime, u8 opcode);

		/*
		* Passed all instruction tests:
		* 01-special
		* 02-interrupts
		* 03-op sp,hl
		* 04-op r,imm
		* 05-op rp
		* 06-op ld r,r
		* 07-jr,jp,call,ret,rst
		* 08-misc instrs
		* 09-op r,r
		* 10-bit ops
		* 11-op a,(hl)
		*
		* Passed other tests:
		* instr_timing
		*/
	};

}