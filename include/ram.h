#pragma once

#include <common.h>

#include <array>

namespace jmpr
{
	class RAM
	{
		// From 0xC000 to 0xDFFF
		std::array<std::array<u8, 0x1000>, 8> _new_wram;
		u8 _wram[0x2000];
		u8 _wram_bank;

		// From 0xFF80 to 0xFFFE
		u8 _hram[0x80];

	public:
		RAM();

		void writeWorkRam(u16 address, u8 data);
		u8 readWorkRam(u16 address);

		void writeHighRam(u16 address, u8 data);
		u8 readHighRam(u16 address);

		u8 getBank() const;
		void setBank(u8 bank);
	};
}