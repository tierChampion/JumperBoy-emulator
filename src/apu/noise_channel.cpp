#include <apu/noise_channel.h>

namespace jmpr {

	NoiseChannel::NoiseChannel(u8 addressSpaceStart) : AudioChannel() {

		_lfsr = 0;
		_clk_shift = 0;
		_lfsr_size = 15;
		_clk_divider = 0;
		_timer = 0;

		_base_vol = 0;
		_vol = _base_vol;
		_rising_envelope = false;
		_envelope_pace = 0;
		_envelope_step = 0;

		_state._right = false;

		_addr_start = addressSpaceStart;
	}

	void NoiseChannel::reset() {}

	void NoiseChannel::update() {

		_timer--;

		if (_timer == 0) {

			u8 shifted = (bit(_lfsr, 0) == bit(_lfsr, 1));

			// Copy of the shifted bit to the 15th and/or 7th bit
			_lfsr |= (shifted << 15);
			if (_lfsr_size == 7)
				_lfsr |= (shifted << 6);

			_lfsr >>= 1; // Right shift of the whole LFSR.

			if (_clk_divider == 0) {
				_timer = (1 << (_clk_shift + 3));
			}
			else {
				_timer = (_clk_divider << (_clk_shift + 4));
			}
		}
	}

	/**
	* Update the envelope effect, which might change the volume.
	*/
	void NoiseChannel::updateEnvelope() {

		if (_envelope_pace > 0) {

			_envelope_step++;

			if (_envelope_step == _envelope_pace) {

				if ((_rising_envelope && _vol < 0xF) || (!_rising_envelope && _vol > 0))
					_vol += (_rising_envelope ? 1 : -1);

				_envelope_step = 0;
			}
		}
	}

	void NoiseChannel::write(u8 address, u8 data) {

		if (address == _addr_start) {

			writeLength(data);
		}
		else if (address == _addr_start + 1) {

			writeVolume(data);
		}
		else if (address == _addr_start + 2) {

			writeFrequency(data);
		}
		else if (address == _addr_start + 3) {

			writeControl(data);
		}
	}

	u8 NoiseChannel::read(u8 address) const { 
		
		u8 out = 0xFF;

		if (address == _addr_start + 1) {

			out = readVolume();
		}
		else if (address == _addr_start + 2) {

			out = readFrequency();
		}
		else if (address == _addr_start + 3) {

			out = readControl();
		}
		
		return out;
	}

	/**
	* Write to the length register (NR41).
	*/
	void NoiseChannel::writeLength(u8 newLength) {

		_length_timer = (newLength & 0b111111);
	}

	/**
	* Write to the volume register (NR42).
	*/
	void NoiseChannel::writeVolume(u8 newVolume) {

		_base_vol = hiNibble(newVolume);

		_rising_envelope = bit(newVolume, 3);

		_envelope_pace = (newVolume & 0b111);
		_envelope_step = 0;

		if (_base_vol == 0 && !_rising_envelope) {

			_state._dac = false;
			_state._active = false;
		}
		else
			_state._dac = true;
	}

	/**
	* Read from the volume register (NR42).
	*/
	u8 NoiseChannel::readVolume() const {

		return (_base_vol << 4) | (_rising_envelope & 0b1000) | _envelope_pace;
	}

	/**
	* Write to the frequency or sound register (NR43).
	*/
	void NoiseChannel::writeFrequency(u8 newSound) {

		_clk_shift = hiNibble(newSound);

		_lfsr_size = (bit(newSound, 3) ? 7 : 15);

		_clk_divider = (newSound & 0b111);

		if (_clk_divider == 0) {
			_timer = (1 << (_clk_shift + 3));
		}
		else {
			_timer = (_clk_divider << (_clk_shift + 4));
		}
	}

	/**
	* Read from the frequency or sound register (NR43).
	*/
	u8 NoiseChannel::readFrequency() const {

		return (_clk_shift << 4) | (15 - _lfsr_size) | _clk_divider;
	}

	/**
	* Write to the control register (NR44).
	*/
	void NoiseChannel::writeControl(u8 newControl) {

		_state._length = bit(newControl, 6);

		// retriggering
		if (bit(newControl, 7) && _state._dac) {

			_state._active = true;

			_lfsr = 0;
			_vol = _base_vol;
			_envelope_step = 0;
		}
	}

	/**
	* Read from the control register (NR44).
	*/
	u8 NoiseChannel::readControl() const {

		return (_state._length & 0b1000000) | 0b10111111;
	}

	/**
	* Compute the current volume of the channel.
	*/
	std::array<float, 2> NoiseChannel::sample() { 
		
		_out[0] = 0;
		_out[1] = 0;

		if (_state._active && _state._dac && !_muted) {

			u8 digitalSignal = bit(_lfsr, 0) * _vol;

			float analogSignal = -(2 * digitalSignal / 16.0f) + 1;

			_out[0] = analogSignal * _state._left;
			_out[1] = analogSignal * _state._right;
		}

		return _out;
	}
}
