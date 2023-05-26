#include <ppu/vmem.h>

#include <ppu/lcd.h>

namespace jmpr {


	// VRAM

	VRAM::VRAM(LCD* lcd) : _vram{ 0 } {

		_lcd = lcd;
	}

	bool VRAM::isAccessible() const {
		return true;
		/*
		return (_lcd->mode() == LCDMode::HBLANK ||
			_lcd->mode() == LCDMode::VBLANK ||
			_lcd->mode() == LCDMode::OAM_SCAN);
		//	*/
	}

	u8 VRAM::read(u16 address) const {

		if (!isAccessible()) return 0xFF;

		return _vram[address - 0x8000];
	}

	void VRAM::write(u16 address, u8 data) {

		if (!isAccessible()) return;

		_vram[address - 0x8000] = data;
	}

	u8 VRAM::ppuRead(u16 address) const {

		return _vram[address - 0x8000];
	}


	// Sprites

	bool Sprite::noPriority() const {

		return bit(_attributes, 7);
	}

	bool Sprite::isFlippedY() const {

		return bit(_attributes, 6);
	}

	bool Sprite::isFlippedX() const {

		return bit(_attributes, 5);
	}

	u8 Sprite::pallet() const {

		return bit(_attributes, 4);
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


	// OAM

	OAM::OAM(LCD* lcd) : _oam{ 0 } {

		_lcd = lcd;
	}

	bool OAM::isAccessible() const {
		return true;
		/*
		return (_lcd->mode() == LCDMode::HBLANK ||
			_lcd->mode() == LCDMode::VBLANK);
		//	*/
	}

	u8 OAM::read(u16 address, bool dmaRead) const {

		if (!dmaRead && !isAccessible()) return 0xFF;

		u8 mappedAddr = (address - 0xFE00) / 4;

		return (_oam[mappedAddr])[address % 4];
	}

	void OAM::write(u16 address, u8 data, bool dmaWrite) {

		if (!dmaWrite && !isAccessible()) return;

		u8 mappedAddr = (address - 0xFE00) / 4;

		(_oam[mappedAddr])[address % 4] = data;
	}

	Sprite OAM::ppuRead(u16 spriteId) const {

		return _oam[spriteId];
	}
}