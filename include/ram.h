#pragma once

#include <common.h>

#include <array>

namespace jmpr
{
	class RAM
	{
		// From 0xC000 to 0xDFFF
		std::array<std::array<u8, 0x1000>, 8> _wram;
		u8 _wram_bank;

		// From 0xFF80 to 0xFFFE
		std::array<u8, 0x80> _hram;

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