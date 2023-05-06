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
	};

}