#pragma once

#include <headers/common.h>
#include <headers/cartridge.h>

namespace jmpr {

	class Bus {

		// To see...
		Cartridge _cartridge;

	public:

		u8 read(u16 address);
		void write(u16 address, u8 data);
	};
}