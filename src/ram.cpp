#include <ram.h>

namespace jmpr
{

	RAM::RAM()
	: _wram_bank(0), _new_wram(std::array<std::array<u8, 0x2000>, 2>())
	{
	}

	void RAM::writeWorkRam(u16 address, u8 data)
	{
		_new_wram[_wram_bank][address - 0xC000] = data;
	}

	u8 RAM::readWorkRam(u16 address)
	{
		return _new_wram[_wram_bank][address - 0xC000];
	}

	void RAM::writeHighRam(u16 address, u8 data)
	{
		_hram[address - 0xFF80] = data;
	}

	u8 RAM::readHighRam(u16 address)
	{
		return _hram[address - 0xFF80];
	}

	u8 RAM::getBank() const
	{
		return 0xFE | _wram_bank;
	}

	void RAM::setBank(u8 bank)
	{
		_wram_bank = bank & 1;
	}
}