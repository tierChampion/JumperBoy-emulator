#include <apu/apu.h>

namespace jmpr {

	// NR50 77 (volume of 7 in both outputs)
	// NR51 F3 (all channels in both, except channel 4 only in left output)
	// NR52 F1 (apu on, channel 1 on)

	APU::APU() : _waveRAM{ 0 } {

		_apu_power = true;

		_channels[0] = AudioChannelState(true, 0b11);
		_channels[1] = AudioChannelState(true, 0b11);
		_channels[2] = AudioChannelState(true, 0b10);
		_channels[3] = AudioChannelState(true, 0b10);

		_left_vol = 0b111;
		_right_vol = 0b111;

		// vol2
	}

	void APU::reboot() {

		// todo
	}

	void APU::updateEffects(bool fallingEdge) {

		if (fallingEdge) {

			_div_apu++;

			// every 2, sound length
			if ((_div_apu & 0b1) == 0) {

			}

			// every 4, ch1 freq sweep
			if ((_div_apu & 0b11) == 0) {

			}

			// every 8, envelope sweep
			if ((_div_apu & 0b111) == 0) {

			}
		}
	}

	void APU::updateChannels() {

		// every m cycle

		_ch2_period_pointer--;

		if (_ch2_period_pointer == 0) {

			_ch2_duty_pointer = (_ch2_duty_pointer + 1) % 8;
			_ch2_period_pointer = 0x800 - _ch2_period_timer;
		}
	}

	// fix read write for specific bit accesibility
	// todo for later

	u8 APU::read(u8 address) {

		if (between(address, 0x30, 0x3F))
			return _waveRAM[address - 0x30];

		return 0xFF;
	}

	void APU::write(u8 address, u8 data) {

		if (between(address, 0x30, 0x3F))
			_waveRAM[address - 0x30] = data;
	}

	// NR52

	u8 APU::getAPUPower() const {

		u8 result = (_apu_power << 7) | 0b01110000;

		for (u8 c = 0; c < AUDIO_CHANNEL_COUNT; c++) {
			result |= _channels[c]._active << c;
		}

		return result;
	}

	void APU::updateAPUPower(u8 newPower) {

		u8 powerStatus = bit(newPower, 7);

		_apu_power = (powerStatus > 0);

		// Power OFF
		if (!_apu_power) {

			// todo: set every single registers in the apu to 0.
		}
	}

	// NR51

	u8 APU::getPanning() const {

		u8 result = 0;

		for (u8 c = 0; c < AUDIO_CHANNEL_COUNT; c++) {

			result |= (bit(_channels[c]._left, 1) << (AUDIO_CHANNEL_COUNT + c)) | (bit(_channels[c]._right, 0) << c);
		}

		return result;
	}

	void APU::updateChannelPanning(u8 newPanning) {

		for (u8 c = 0; c < AUDIO_CHANNEL_COUNT; c++) {

			_channels[c]._left = bit(newPanning, AUDIO_CHANNEL_COUNT + c) << 1;
			_channels[c]._right = bit(newPanning, c);
		}
	}

	// NR50

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