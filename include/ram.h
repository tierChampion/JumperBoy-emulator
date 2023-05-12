#pragma once

#include <common.h>

namespace jmpr {

	class RAM {

		// From 0xC000 to 0xDFFF
		u8 _wram[0x2000];

		// From 0xFF80 to 0xFFFE
		u8 _hram[0x80];

	public:

		void writeWorkRam(u16 address, u8 data);
		u8 readWorkRam(u16 address);

		void writeHighRam(u16 address, u8 data);
		u8 readHighRam(u16 address);
	};
}