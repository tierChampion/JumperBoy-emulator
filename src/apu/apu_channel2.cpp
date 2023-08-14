#include <apu/apu.h>

#include <array>

namespace jmpr {

	/*
	* TODOS:
	*
	* initialisation of the channel2
	* proper writing and reading behaviour
	* simple signal generation
	* adding of the enveloppe and length functionnalities
	* adding of sweep for channel 1
	*/

	// NR21 3F (wave duty of 00, initial length timer of 111111)
	// NR22 00 (initial volume of 0, envelope decrease and no sweep. DAC off)
	// NR23 FF (low 8 bits of period is 11111111)
	// NR24 BF (trigger on, sound length disabled, upper bits of period is 111)

	/*
	* Initialise Channel 2 (Pulse with no sweep).
	*
	* todo
	*/
	void APU::initChannel2() {

		_channels[1] = AudioChannelState();
		_channels[1]._active = false;
		_channels[1]._dac = false;
		_channels[1]._left = true;
		_channels[1]._right = true;

		_ch2_duty = 0;
		_ch2_duty_pointer = 0;
		_ch2_vol = 0;
		_ch2_period_timer = 0x7FF;
		_ch2_period_pointer = 1;
	}

	void APU::writeChannel2(u8 address, u8 data) {

		switch (address) {
		case 0x16: writeChannel2Lengths(data); break;
		case 0x17: writeChannel2Volume(data); break;
		case 0x18: writeChannel2PeriodControl(0, data); break;
		case 0x19: writeChannel2PeriodControl(1, data); break;
		}
	}

	u8 APU::readChannel2(u8 address) {

		switch (address) {
		case 0x16: return readChannel2Duty(); break;
		case 0x17: return readChannel2Volume(); break;
		case 0x19: return readChannel2Control(); break;
		}

		return 0xFF;
	}

	/*
	* Write to the length register (NR21).
	*
	* todo: length timer is not yet implemented
	*/
	void APU::writeChannel2Lengths(u8 newLength) {

		_ch2_duty = (newLength & 0b11000000) >> 6;
		_ch2_duty_pointer = 0;
	}

	/*
	* Read the length register (NR21).
	*/
	u8 APU::readChannel2Duty() const {

		return (_ch2_duty << 6) | 0b111111;
	}

	/*
	* Write to the volume and envelope register (NR22).
	*
	* todo: envelope is not yet implemented and dac closing might need mods.
	*/
	void APU::writeChannel2Volume(u8 newVolume) {

		_ch2_vol = hiNibble(newVolume);

		// dac turned off, channel turned off
		if ((newVolume & 0b11111000) == 0) {

			_channels[1]._dac = false;
			// todo bunch of other stuff
		}
		else _channels[1]._dac = true;
	}

	/*
	* Read from the volume and envelope register (NR22).
	*
	* todo: envelope is not yet implemented
	*/
	u8 APU::readChannel2Volume() const {

		return (_ch2_vol << 5) | 0b11111;
	}

	/*
	* Write to the period and the control registers (NR23 and NR24).
	*
	* todo: sound length is not yet implemented
	*/
	void APU::writeChannel2PeriodControl(u8 selection, u8 newPeriodControl) {

		// NR23
		if (selection == 0) {

			_ch2_period_timer = (_ch2_period_timer & 0x700) | newPeriodControl;
			_ch2_period_pointer = 0x800 - _ch2_period_timer;
		}

		// NR24
		else {

			_ch2_period_timer = (newPeriodControl & 0b111) | (_ch2_period_timer & 0xFF);
			_ch2_period_pointer = 0x800 - _ch2_period_timer;

			// retriggering
			if (bit(newPeriodControl, 7) && _channels[1]._dac) {

				// set the volume change, dac state
				_channels[1]._active = true;
			}
		}
	}

	/*
	* Read from the control register (NR24).
	*
	* todo: sound length is not yet implemented
	*/
	u8 APU::readChannel2Control() const {

		return 0;
	}

	static const std::array<u8, 32> PULSE_DUTY_CYCLES = {
			0, 1, 1, 1, 1, 1, 1, 1, // 12.5% low
			0, 0, 1, 1, 1, 1, 1, 1, // 25% low
			0, 0, 0, 0, 1, 1, 1, 1, // 50% low
			0, 0, 0, 0, 0, 0, 1, 1  // 75% low
	};

	/*
	* Generate a single analog sample from the channel 2.
	*/
	float APU::generateChannel2() const {

		float analogSignal = 0;

		if (_channels[1]._active) {

			// high or low
			u8 signal = PULSE_DUTY_CYCLES[8 * _ch2_duty + _ch2_duty_pointer];

			u8 digitalSignal = signal * (_ch2_vol >> 4);

			analogSignal = -(digitalSignal / 7.5f) + 1;
		}

		return analogSignal;
	}
}
