#include <ppu/vram.h>

namespace jmpr {

	VRAM::VRAM() : _vram{ 0 }, _oam{ 0 } {}

	u8 VRAM::readVRAM(u16 address) const {

		return _vram[address - 0x8000];
	}

	void VRAM::writeVRAM(u16 address, u8 data) {
		_vram[address - 0x8000] = data;
	}

	u8 Sprite::operator[](int i) const {
		if (i == 0) {
			return _ypos;
		}
		else if (i == 1) {
			return _xpos;
		}
		else if (i == 2) {
			return _tile_id;
		}
		else {
			return _attributes;
		}
	}

	u8& Sprite::operator[](int i) {
		if (i == 0) {
			return _ypos;
		}
		else if (i == 1) {
			return _xpos;
		}
		else if (i == 2) {
			return _tile_id;
		}
		else {
			return _attributes;
		}
	}

	u8 VRAM::readOAM(u16 address) const {

		u8 mappedAddr = (address - 0xFE00) / 4;

		return (_oam[mappedAddr])[mappedAddr % 4];
	}

	void VRAM::writeOAM(u16 address, u8 data) {

		u8 mappedAddr = (address - 0xFE00) / 4;

		(_oam[mappedAddr])[mappedAddr % 4] = data;
	}

}