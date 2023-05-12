#include <apu/apu.h>

namespace jmpr {

	APU::APU() : _waveRAM{ 0 } {

		_NR10 = 0x80;
		_NR11 = 0xBF;
		_NR12 = 0x3F;
		_NR13 = 0xFF;
		_NR14 = 0xBF;

		_NR21 = 0x3F;
		_NR22 = 0x00;
		_NR23 = 0xFF;
		_NR24 = 0xBF;

		_NR30 = 0x7F;
		_NR31 = 0xFF;
		_NR32 = 0x9F;
		_NR33 = 0xFF;
		_NR34 = 0xBF;

		_NR41 = 0xFF;
		_NR42 = 0x00;
		_NR43 = 0x00;
		_NR44 = 0xBF;

		_NR50 = 0x77;
		_NR51 = 0xF3;
		_NR52 = 0xF1;
	}

	// fix read write for specific bit accesibility
	// todo for later

	u8 APU::read(u8 address) {

		switch (address) {

		case 0x10: return _NR10; break;
		case 0x11: return _NR11; break;
		case 0x12: return _NR12; break;
		case 0x13: return _NR13; break;
		case 0x14: return _NR14; break;

		case 0x16: return _NR21; break;
		case 0x17: return _NR22; break;
		case 0x18: return _NR23; break;
		case 0x19: return _NR24; break;

		case 0x1A: return _NR30; break;
		case 0x1B: return _NR31; break;
		case 0x1C: return _NR32; break;
		case 0x1D: return _NR33; break;
		case 0x1E: return _NR34; break;

		case 0x20: return _NR41; break;
		case 0x21: return _NR42; break;
		case 0x22: return _NR43; break;
		case 0x23: return _NR44; break;

		case 0x24: return _NR50; break;
		case 0x25: return _NR51; break;
		case 0x26: return _NR52; break;
		}

		if (between(address, 0x30, 0x3F))
			return _waveRAM[address - 0x30];

		return 0xFF;
	}

	void APU::write(u8 address, u8 data) {

		switch (address) {

		case 0x10: _NR10 = data; break;
		case 0x11: _NR11 = data; break;
		case 0x12: _NR12 = data; break;
		case 0x13: _NR13 = data; break;
		case 0x14: _NR14 = data; break;

		case 0x16: _NR21 = data; break;
		case 0x17: _NR22 = data; break;
		case 0x18: _NR23 = data; break;
		case 0x19: _NR24 = data; break;

		case 0x1A: _NR30 = data; break;
		case 0x1B: _NR31 = data; break;
		case 0x1C: _NR32 = data; break;
		case 0x1D: _NR33 = data; break;
		case 0x1E: _NR34 = data; break;

		case 0x20: _NR41 = data; break;
		case 0x21: _NR42 = data; break;
		case 0x22: _NR43 = data; break;
		case 0x23: _NR44 = data; break;

		case 0x24: _NR50 = data; break;
		case 0x25: _NR51 = data; break;
		case 0x26: _NR52 = data; break;
		}

		if (between(address, 0x30, 0x3F))
			_waveRAM[address - 0x30] = data;
	}
}