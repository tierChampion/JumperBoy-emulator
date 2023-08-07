#include <apu/apu.h>

#include <array>

namespace jmpr {

	static const std::array<u8, 32> DUTY_CYCLES = {
		0, 1, 1, 1, 1, 1, 1, 1, // 12.5% low
		0, 0, 1, 1, 1, 1, 1, 1, // 25% low
		0, 0, 0, 0, 1, 1, 1, 1, // 50% low
		0, 0, 0, 0, 0, 0, 1, 1  // 75% low
	};

	APU::APU() : _waveRAM{ 0 } {

		_apu_power = true;

		_channels[0] = AudioChannel(true, 0b11);
		_channels[1] = AudioChannel(true, 0b11);
		_channels[2] = AudioChannel(true, 0b10);
		_channels[3] = AudioChannel(true, 0b10);

		_left_vol = 0b000;
		_right_vol = 0b000;
	}

	void APU::reboot() {

		// todo
	}

	void APU::update(u8 oldEdge, u8 newEdge) {

		if (oldEdge == 1 && newEdge == 0) {

			_div_apu++;

			// every 2, sound length
			if ((_div_apu & 1) == 0) {

				for (u8 c = 0; c < AUDIO_CHANNEL_COUNT; c++) {
					_channels[c].update();
				}
			}

			// every 4, ch1 freq sweep
			// every 8, envelope sweep
		}
	}

	void AudioChannel::update() {

		if (_gen_power && _len_enabled) {

			_len_timer++;

			if (_len_timer == _timer_end) {

				_len_timer == _timer_begin;
				_gen_power = false;
			}
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