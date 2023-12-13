#include <apu/square_channel.h>

namespace jmpr
{

	SquareChannel::SquareChannel(bool activate, u8 addressSpaceStart, bool allowSweep) : AudioChannel()
	{

		_duty = (activate ? 0b10 : 0b00);
		_duty_step = 0;

		_base_vol = (activate ? 0xF : 0);
		_vol = _base_vol;
		_rising_envelope = false;
		_envelope_pace = 0b11;
		_envelope_step = 0;

		_freq = 0;
		_timer = 0x800 * 4;

		_state._active = activate;
		_state._dac = activate;
		_state._left = true;
		_state._right = true;

		_addr_start = addressSpaceStart;
		_allow_sweep = allowSweep;
	}

	void SquareChannel::reset() {}

	void SquareChannel::update()
	{

		_timer--;

		if (_timer == 0)
		{

			_duty_step = (_duty_step + 1) % 8;
			_timer = (0x800 - _freq) * 4;
		}
	}

	/**
	 * Update the sweep effect, which might change the period.
	 */
	void SquareChannel::updateSweep()
	{
		if (_sweep_pace > 0 && _allow_sweep)
		{
			u16 variation = _freq >> _sweep_step;

			if (_sweep_dir == 0)
			{
				_freq += variation;

				if (_freq > 0x7FF)
					_state._active = false;
			}
			else
			{
				_freq -= variation;
			}
		}
	}

	/**
	 * Update the envelope effect, which might change the volume.
	 */
	void SquareChannel::updateEnvelope()
	{

		if (_envelope_pace > 0)
		{

			_envelope_step++;

			if (_envelope_step == _envelope_pace)
			{

				if ((_rising_envelope && _vol < 0xF) || (!_rising_envelope && _vol > 0))
					_vol += (_rising_envelope ? 1 : -1);

				_envelope_step = 0;
			}
		}
	}

	void SquareChannel::write(u8 address, u8 data)
	{

		if (address == _addr_start)
		{
			writeSweep(data);
		}
		else if (address == _addr_start + 1)
		{
			writeLengths(data);
		}
		else if (address == _addr_start + 2)
		{
			writeVolume(data);
		}
		else if (address == _addr_start + 3)
		{
			writePeriodControl(0, data);
		}
		else if (address == _addr_start + 4)
		{
			writePeriodControl(1, data);
		}
	}

	u8 SquareChannel::read(u8 address) const
	{

		u8 out = 0xFF;

		if (address == _addr_start)
		{
			out = readSweep();
		}
		else if (address == _addr_start + 1)
		{
			out = readDuty();
		}
		else if (address == _addr_start + 2)
		{
			out = readVolume();
		}
		else if (address == _addr_start + 4)
		{
			out = readControl();
		}

		return out;
	}

	/*
	 * Write to the sweep register (NRx0).
	 */
	void SquareChannel::writeSweep(u8 newSweep)
	{
		_sweep_pace = (newSweep & 0x70) >> 4;
		_sweep_dir = (newSweep & 0x08) >> 3;
		_sweep_step = (newSweep & 0x07) >> 3;
	}

	/*
	 * Read the sweep register (NRx0).
	 *
	 * todo
	 */
	u8 SquareChannel::readSweep() const
	{
		u8 ret = 0xFF;

		if (_allow_sweep)
		{
			ret = (_sweep_pace << 4) | (_sweep_dir << 3) | (_sweep_step);
		}

		return ret;
	}

	/*
	 * Write to the length register (NRx1).
	 */
	void SquareChannel::writeLengths(u8 newLength)
	{
		_duty = (newLength & 0b11000000) >> 6;
		_duty_step = 0;

		_length_timer = (newLength & 0b111111);
	}

	/*
	 * Read the length register (NRx1).
	 */
	u8 SquareChannel::readDuty() const
	{

		return (_duty << 6) | 0b111111;
	}

	/*
	 * Write to the volume and envelope register (NRx2).
	 *
	 * todo: envelope is not yet implemented and dac closing might need mods.
	 */
	void SquareChannel::writeVolume(u8 newVolume)
	{

		_base_vol = hiNibble(newVolume);

		_rising_envelope = bit(newVolume, 3);
		_envelope_pace = (newVolume & 0b111);
		_envelope_step = 0;

		// dac turned off, channel turned off
		if ((newVolume & 0b11110000) == 0)
		{

			_state._dac = false;
			_state._active = false;
		}
		else
			_state._dac = true;
	}

	/*
	 * Read from the volume and envelope register (NRx2).
	 *
	 * todo: envelope is not yet implemented
	 */
	u8 SquareChannel::readVolume() const
	{

		return (_base_vol << 4) | (_rising_envelope << 3) | (_envelope_pace);
	}

	/*
	 * Write to the period and the control registers (NRx3 and NRx4).
	 *
	 * todo: sound length is not yet implemented
	 */
	void SquareChannel::writePeriodControl(u8 selection, u8 newPeriodControl)
	{

		// NRx3
		if (selection == 0)
		{

			_freq = (_freq & 0x700) | newPeriodControl;
			_timer = (0x800 - _freq) * 4;
		}

		// NRx4
		else
		{

			_freq = ((newPeriodControl & 0b111) << 8) | (_freq & 0xFF);
			_timer = (0x800 - _freq) * 4;

			_state._length = bit(newPeriodControl, 6);

			// retriggering
			if (bit(newPeriodControl, 7) && _state._dac)
			{

				_state._active = true;

				_vol = _base_vol;
				_envelope_step = 0;
			}
		}
	}

	/*
	 * Read from the control register (NRx4).
	 *
	 * todo: sound length is not yet implemented
	 */
	u8 SquareChannel::readControl() const
	{

		return 0xBF | ((u8)_state._length << 6);
	}

	static const std::array<u8, 32> PULSE_DUTY_CYCLES = {
		0, 0, 0, 0, 0, 0, 0, 1, // 12.5% low
		1, 0, 0, 0, 0, 0, 0, 1, // 25% low
		1, 0, 0, 0, 0, 1, 1, 1, // 50% low
		0, 1, 1, 1, 1, 1, 0, 0	// 75% low
	};

	/**
	 * Compute the current volume of the channel.
	 */
	void SquareChannel::addSample(float &leftSample, float &rightSample) const
	{

		if (_state._active && _state._dac && !_muted)
		{

			// high or low
			u8 signal = PULSE_DUTY_CYCLES[8 * _duty + _duty_step];

			u8 digitalSignal = signal * _vol;

			float analogSignal = -(2 * digitalSignal / 16.0f) + 1;

			leftSample += analogSignal * _state._left;
			rightSample += analogSignal * _state._right;
		}
	}
}
