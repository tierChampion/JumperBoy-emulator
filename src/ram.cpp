#include <ram.h>

namespace jmpr
{

	RAM::RAM()
	: _wram_bank(0), _wram(std::array<std::array<u8, 0x1000>, 8>()), _hram(std::array<u8, 0x80>())
	{
	}

	void RAM::writeWorkRam(u16 address, u8 data)
	{
		if (address < 0xD000) {
			_wram[0][address - 0xC000] = data;
		}
		else {
			_wram[_wram_bank + 1][address - 0xD000] = data;
		}
	}

	u8 RAM::readWorkRam(u16 address)
	{
		if (address < 0xD000) {
			return _wram[0][address - 0xC000];
		}
		else {
			return _wram[_wram_bank + 1][address - 0xD000];
		}
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
		return 0xF8 | _wram_bank;
	}

	void RAM::setBank(u8 bank)
	{
		_wram_bank = bank & 0x7;
	}
}