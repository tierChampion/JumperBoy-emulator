#include <ram.h>

namespace jmpr {

	void RAM::writeWorkRam(u16 address, u8 data) {

		_wram[address - 0xC000] = data;
	}

	u8 RAM::readWorkRam(u16 address) {

		return _wram[address - 0xC000];
	}

	void RAM::writeHighRam(u16 address, u8 data) {

		_hram[address - 0xFF80] = data;
	}

	u8 RAM::readHighRam(u16 address) {

		return _hram[address - 0xFF80];
	}
}