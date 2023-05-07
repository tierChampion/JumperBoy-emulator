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

	public:

		Debugger(Bus* bus);

		void update();
		void log();

		/*
		* Passed tests:
		* 01-special
		*
		* 03-op sp,hl
		* 04-op r,imm
		* 05-op rp
		* 06-op ld r,r
		*
		* 08-misc instrs
		* 09-op r,r
		* 10-bit ops
		* 11-op a,(hl)
		*/
	};

}