#include <ram.h>

namespace jmpr {

	void Ram::writeWorkRam(u16 address, u8 data) {

		_wram[address - 0xC000] = data;
	}

	u8 Ram::readWorkRam(u16 address) {

		return _wram[address - 0xC000];
	}

	void Ram::writeHighRam(u16 address, u8 data) {

		_hram[address - 0xFF80] = data;
	}

	u8 Ram::readHighRam(u16 address) {

		return _hram[address - 0xFF80];
	}
}