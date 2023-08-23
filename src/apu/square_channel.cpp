#include <apu/square_channel.h>

namespace jmpr {

	SquareChannel::SquareChannel() : AudioChannel() {

		_duty = 0;
		_duty_step = 0; // where in the cycle you are. increments everytime the period timer goes to 0
		_vol = 0;
		_freq = 0; // length of a single step of duty cycles. decrements every m_cycle
		_timer = 0x800 * 4;

		_out = std::array<float, 2>();

		_state._left = true;
		_state._right = true;
	}

	void SquareChannel::reset() {}

	void SquareChannel::update() {

		_timer--;

		// todo enveloppe, sweep, length timer, etc...

		if (_timer == 0) {

			_duty_step = (_duty_step + 1) % 8;
			_timer = (0x800 - _freq) * 4;
		}
	}

	// switch address later for channel 1 support

	void SquareChannel::write(u8 address, u8 data) {

		switch (address) {
		case 0x16: writeChannel2Lengths(data); break;
		case 0x17: writeChannel2Volume(data); break;
		case 0x18: writeChannel2PeriodControl(0, data); break;
		case 0x19: writeChannel2PeriodControl(1, data); break;
		}
	}

	u8 SquareChannel::read(u8 address) const {

		switch (address) {
		case 0x16: return readChannel2Duty(); break;
		case 0x17: return readChannel2Volume(); break;
		case 0x19: return readChannel2Control(); break;
		}

		return 0xFF;
	}

	/*
	* Write to the length register (NRx1).
	*
	* todo: length timer is not yet implemented
	*/
	void SquareChannel::writeChannel2Lengths(u8 newLength) {

		_duty = (newLength & 0b11000000) >> 6;
		_duty_step = 0;
	}

	/*
	* Read the length register (NRx1).
	*/
	u8 SquareChannel::readChannel2Duty() const {

		return (_duty << 6) | 0b111111;
	}

	/*
	* Write to the volume and envelope register (NRx2).
	*
	* todo: envelope is not yet implemented and dac closing might need mods.
	*/
	void SquareChannel::writeChannel2Volume(u8 newVolume) {

		_vol = hiNibble(newVolume);

		// dac turned off, channel turned off
		if ((newVolume & 0b11110000) == 0) {

			_state._dac = false;
			// todo bunch of other stuff
			_state._active = false;
		}
		else _state._dac = true;
	}

	/*
	* Read from the volume and envelope register (NRx2).
	*
	* todo: envelope is not yet implemented
	*/
	u8 SquareChannel::readChannel2Volume() const {

		return (_vol << 5) | 0b11111;
	}

	/*
	* Write to the period and the control registers (NRx3 and NRx4).
	*
	* todo: sound length is not yet implemented
	*/
	void SquareChannel::writeChannel2PeriodControl(u8 selection, u8 newPeriodControl) {

		// NRx3
		if (selection == 0) {

			_freq = (_freq & 0x700) | newPeriodControl;
			_timer = (0x800 - _freq) * 4;
		}

		// NRx4
		else {

			_freq = ((newPeriodControl & 0b111) << 8) | (_freq & 0xFF);
			_timer = (0x800 - _freq) * 4;

			// retriggering
			if (bit(newPeriodControl, 7) && _state._dac) {

				// set the volume change, dac state
				_state._active = true;
			}
		}
	}

	/*
	* Read from the control register (NRx4).
	*
	* todo: sound length is not yet implemented
	*/
	u8 SquareChannel::readChannel2Control() const {

		return 0;
	}

	static const std::array<u8, 32> PULSE_DUTY_CYCLES = {
			0, 0, 0, 0, 0, 0, 0, 1, // 12.5% low
			1, 0, 0, 0, 0, 0, 0, 1, // 25% low
			1, 0, 0, 0, 0, 1, 1, 1, // 50% low
			0, 1, 1, 1, 1, 1, 0, 0  // 75% low
	};

	std::array<float, 2> SquareChannel::sample() {

		_out[0] = 0;
		_out[1] = 0;

		if (_state._active && _state._dac) {

			// high or low
			u8 signal = PULSE_DUTY_CYCLES[8 * _duty + _duty_step];

			u8 digitalSignal = signal * _vol;

			float analogSignal = -(digitalSignal / 7.5f) + 1;

			_out[0] = analogSignal * _state._left;
			_out[1] = analogSignal * _state._right;

			return _out;
		}

		return _out;
	}
}
