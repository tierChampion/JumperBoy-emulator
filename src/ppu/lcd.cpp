#include <ppu/lcd.h>

#include <cpu/interrupt.h>

#include <array>

namespace jmpr {

	LCD::LCD(InterruptHandler* intHandler) {

		_inter_handler = intHandler;

		_lcdc = 0x91;
		_ly = 0x00;
		_lyc = 0x00;
		_status = 0x85;

		_scy = 0x00;
		_scx = 0x00;
		_wy = 0x00;
		_wx = 0x00;

		_bgp = 0xFC;
		_obp[0] = 0xFF;
		_obp[1] = 0xFF;

		// gbc only registers... 
		_bcps_bgpi = 0xFF;
		_bcpd_bgpd = 0xFF;
		_ocps_obpi = 0xFF;
		_ocpd_obpd = 0xFF;
	}

	void LCD::reset() {

		setMode(LCDMode::OAM_SCAN);
		_ly = 0;
	}

	u8 LCD::read(u8 address) const {

		switch (address) {
		case 0x40: return _lcdc; break;
		case 0x41: return _status; break;
		case 0x42: return _scy; break;
		case 0x43: return _scx; break;
		case 0x44: return _ly; break;
		case 0x45: return _lyc; break;
		case 0x47: return _bgp; break;
		case 0x48: return _obp[0]; break;
		case 0x49: return _obp[1]; break;
		case 0x4A: return _wy; break;
		case 0x4B: return _wx; break;
		}

		return 0xFF;
	}

	void LCD::write(u8 address, u8 data) {

		switch (address) {
		case 0x40: _lcdc = data; break;
		case 0x41: _status = data; break;
		case 0x42: _scy = data; break;
		case 0x43: _scx = data; break;
		case 0x45: _lyc = data; break;
		case 0x47: _bgp = data; break;
		case 0x48: _obp[0] = data; break;
		case 0x49: _obp[1] = data; break;
		case 0x4A: _wy = data; break;
		case 0x4B: _wx = data; break;
		}
	}

	void LCD::jumpScanline() {

		_ly++;

		bool equal = (_ly == _lyc);
		setBit(_status, 2, equal);

		if (equal && bit(_status, 6)) {

			// request an interrupt
			_inter_handler->requestInterrupt(InterruptType::STAT);
		}
	}

	u8 LCD::getScanline() const {
		return _ly;
	}

	u8 LCD::getBGScrollX() const {
		return _scx;
	}

	u8 LCD::getBGScrollY() const {
		return _scy;
	}

	u8 LCD::getWindowX() const {
		return _wx;
	}

	u8 LCD::getWindowY() const {
		return _wy;
	}

	// LCDC

	bool LCD::lcdEnabled() const {
		return bit(_lcdc, 7);
	}

	u16 LCD::windowTileMapAreaBegin() const {
		return bit(_lcdc, 6) ? 0x9C00 : 0x9800;
	}

	bool LCD::windowEnabled() const {
		return bit(_lcdc, 5);
	}

	u16 LCD::tileDataAreaBegin() const {
		return bit(_lcdc, 4) ? 0x8000 : 0x8800;
	}

	u16 LCD::bgTileMapAreaBegin() const {
		return bit(_lcdc, 3) ? 0x9C00 : 0x9800;
	}

	u8 LCD::objSize() const {
		return 1 << (3 + bit(_lcdc, 2));
	}

	bool LCD::objEnabled() const {
		return bit(_lcdc, 1);
	}

	bool LCD::bgWindowPriority() const {
		return bit(_lcdc, 0);
	}

	// STAT

	bool LCD::statInterruptTypeEnabled(LCDMode source) const {

		// Interrupts are globally enabled and for the current mode
		return bit(_status, ((u8)source) + 3);
	}

	LCDMode LCD::mode() const {
		return LCDMode(_status & 0b11);
	}

	void LCD::setMode(LCDMode mode) {
		_status = (_status & 0xFC) | ((u8)mode & 0b11);
	}

	// PALLETS IN ABGR FORMAT

	inline static const std::array<u32, 4> LCD_PALLET = {
		///*
		// Green pallet
		0xFF0FBC9B,
		0xFF0FAC8B,
		0xFF306230,
		0xFF0F380F,
		//*/
		/*
		// Gray pallet
		0xFFFFFFFF,
		0xFFAAAAAA,
		0xFF555555,
		0xFF000000,
		//*/
	};

	u32 LCD::getBGWindowColor(u8 index) const {

		u8 colorId = (_bgp >> (2 * index)) & 0b11;

		return LCD_PALLET[colorId];
	}

	u32 LCD::getOBJcolor(u8 objIndex, u8 colIndex) const {

		if (colIndex == 0) {
			// Color index 0 is always transparent
			return 0x00000000;
		}

		u8 colorId = (_obp[objIndex] >> (2 * colIndex)) & 0b11;

		return LCD_PALLET[colorId];
	}
}