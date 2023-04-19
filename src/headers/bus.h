#pragma once

#include <headers/common.h>

namespace jmpr {

	class Bus {

		// To see...

	public:

		u8 bus_read(u16 address);
		void bus_write(u16 address, u8 data)
	};
}