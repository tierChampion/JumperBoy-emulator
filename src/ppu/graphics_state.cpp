#include <ppu/graphics_state.h>

namespace jmpr {

	GraphicsState::GraphicsState() {

		_lcdc = 0x91;
		_ly = 0x00;
		_lyc = 0x00;
		_status = 0x85;

		_scy = 0x00;
		_scx = 0x00;
		_wy = 0x00;
		_wx = 0x00;

		_bgp = 0xFC;
		_obp0 = 0xFF;
		_obp1 = 0xFF;

		// gbc only registers... 
		_bcps_bgpi = 0xFF;
		_bcpd_bgpd = 0xFF;
		_ocps_obpi = 0xFF;
		_ocpd_obpd = 0xFF;
	}

	u8 GraphicsState::read(u8 address) {

		switch (address) {
		case 0x40: return _lcdc; break;
		case 0x41: return _status; break;
		case 0x42: return _scy; break;
		case 0x43: return _scx; break;
		case 0x44: return _ly++; break; // _ly; break;
		case 0x45: return _lyc; break;
		case 0x47: return _bgp; break;
		case 0x48: return _obp0; break;
		case 0x49: return _obp1; break;
		case 0x4A: return _wy; break;
		case 0x4B: return _wx; break;
		}

		return 0xFF;
	}

	void GraphicsState::write(u8 address, u8 data) {

		switch (address) {
		case 0x40: _lcdc = data; break;
		case 0x41: _status = data; break;
		case 0x42: _scy = data; break;
		case 0x43: _scx = data; break;
		case 0x44: _ly = data; break;
		case 0x45: _lyc = data; break;
		case 0x47: _bgp = data; break;
		case 0x48: _obp0 = data; break;
		case 0x49: _obp1 = data; break;
		case 0x4A: _wy = data; break;
		case 0x4B: _wx = data; break;
		}
	}
}