#include <apu/apu.h>

namespace jmpr {

	APU::APU() : _waveRAM{ 0 } {

		_apu_power = true;

		_channels[0] = AudioChannel(true, 0b11);
		_channels[1] = AudioChannel(true, 0b11);
		_channels[2] = AudioChannel(true, 0b10);
		_channels[3] = AudioChannel(true, 0b10);

		_left_vol = 0b000;
		_right_vol = 0b000;

		_registers._NR10 = 0x80;
		_registers._NR11 = 0xBF;
		_registers._NR12 = 0x3F;
		_registers._NR13 = 0xFF;
		_registers._NR14 = 0xBF;

		_registers._NR21 = 0x3F;
		_registers._NR22 = 0x00;
		_registers._NR23 = 0xFF;
		_registers._NR24 = 0xBF;

		_registers._NR30 = 0x7F;
		_registers._NR31 = 0xFF;
		_registers._NR32 = 0x9F;
		_registers._NR33 = 0xFF;
		_registers._NR34 = 0xBF;

		_registers._NR41 = 0xFF;
		_registers._NR42 = 0x00;
		_registers._NR43 = 0x00;
		_registers._NR44 = 0xBF;

		_registers._NR50 = 0x77;
		_registers._NR51 = 0xF3;
		_registers._NR52 = 0xF1;
	}

	// fix read write for specific bit accesibility
	// todo for later

	u8 APU::read(u8 address) {

		switch (address) {

		case 0x10: return _registers._NR10; break;
		case 0x11: return _registers._NR11; break;
		case 0x12: return _registers._NR12; break;
		case 0x13: return _registers._NR13; break;
		case 0x14: return _registers._NR14; break;

		case 0x16: return _registers._NR21; break;
		case 0x17: return _registers._NR22; break;
		case 0x18: return _registers._NR23; break;
		case 0x19: return _registers._NR24; break;

		case 0x1A: return _registers._NR30; break;
		case 0x1B: return _registers._NR31; break;
		case 0x1C: return _registers._NR32; break;
		case 0x1D: return _registers._NR33; break;
		case 0x1E: return _registers._NR34; break;

		case 0x20: return _registers._NR41; break;
		case 0x21: return _registers._NR42; break;
		case 0x22: return _registers._NR43; break;
		case 0x23: return _registers._NR44; break;

		case 0x24: return _registers._NR50; break;
		case 0x25: return _registers._NR51; break;
		case 0x26: return _registers._NR52; break;
		}

		if (between(address, 0x30, 0x3F))
			return _waveRAM[address - 0x30];

		return 0xFF;
	}

	void APU::write(u8 address, u8 data) {

		switch (address) {

		case 0x10: _registers._NR10 = data; break;
		case 0x11: _registers._NR11 = data; break;
		case 0x12: _registers._NR12 = data; break;
		case 0x13: _registers._NR13 = data; break;
		case 0x14: _registers._NR14 = data; break;

		case 0x16: _registers._NR21 = data; break;
		case 0x17: _registers._NR22 = data; break;
		case 0x18: _registers._NR23 = data; break;
		case 0x19: _registers._NR24 = data; break;

		case 0x1A: _registers._NR30 = data; break;
		case 0x1B: _registers._NR31 = data; break;
		case 0x1C: _registers._NR32 = data; break;
		case 0x1D: _registers._NR33 = data; break;
		case 0x1E: _registers._NR34 = data; break;

		case 0x20: _registers._NR41 = data; break;
		case 0x21: _registers._NR42 = data; break;
		case 0x22: _registers._NR43 = data; break;
		case 0x23: _registers._NR44 = data; break;

		case 0x24: _registers._NR50 = data; break;
		case 0x25: _registers._NR51 = data; break;
		case 0x26: _registers._NR52 = data; break;
		}

		if (between(address, 0x30, 0x3F))
			_waveRAM[address - 0x30] = data;
	}

	u8 APU::getAPUPower() const {

		u8 result = (_apu_power << 7) | 0b01110000;

		for (u8 c = 0; c < AUDIO_CHANNEL_COUNT; c++) {
			result |= _channels[c]._gen_power << c;
		}

		return result;
	}

	void APU::handleAPUPower(u8 newPower) {

		u8 powerStatus = bit(newPower, 7);

		_apu_power = (powerStatus > 0);
		_registers._NR52 = setBit(_registers._NR52, 7, powerStatus);

		// Power OFF
		if (!_apu_power) {

			// todo: set all registers to 0.


		}
	}

	void APU::updateChannelPanning(u8 newPanning) {

		for (u8 c = 0; c < AUDIO_CHANNEL_COUNT; c++) {

			_channels[c]._panning = (bit(newPanning, AUDIO_CHANNEL_COUNT + c) << 1) | (bit(newPanning, c));
		}
	}

	u8 APU::getPanning() const {

		u8 result = 0;

		for (u8 c = 0; c < AUDIO_CHANNEL_COUNT; c++) {

			result |= (bit(_channels[c]._panning, 1) << (AUDIO_CHANNEL_COUNT + c)) | (bit(_channels[c]._panning, 0) << c);
		}

		return result;
	}

	void APU::updateMasterVolume(u8 newVol) {

		_left_vol = (newVol & 0b01110000) >> 4;
		_right_vol = (newVol & 0b00000111);
	}

	u8 APU::getMasterVolume() const {

		u8 result = 0;

		result |= _left_vol << 4;
		result |= _right_vol;

		return result;
	}
}