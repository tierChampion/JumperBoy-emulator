#include <apu/wave_channel.h>

namespace jmpr
{
	WaveChannel::WaveChannel(u8 addressSpaceStart) : AudioChannel(), _waveRAM(std::array<u8, 0x10>())
	{

		_step = 0;
		_freq = 0;
		_timer = 0x800 * 2;

		_output_level = 0;

		_state._left = true;
		_state._right = true;

		_addr_start = addressSpaceStart;
	}

	void WaveChannel::reset() {}

	void WaveChannel::update()
	{
		_timer--;

		if (_timer == 0)
		{

			_step = (_step + 1) % 0x20; // to see
			_timer = (0x800 - _freq) * 2;
		}
	}

	void WaveChannel::write(u8 address, u8 data)
	{

		if (address == _addr_start)
		{
			writeDAC(data);
		}
		else if (address == _addr_start + 1)
		{
			writeLength(data);
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

	u8 WaveChannel::read(u8 address) const
	{

		u8 out = 0xFF;

		if (address == _addr_start)
		{
			out = readDAC();
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

	void WaveChannel::writeToRAM(u8 address, u8 newSamples)
	{

		if (between(address, 0x30, 0x3F))
		{
			_waveRAM[address - 0x30] = newSamples;
		}
	}

	u8 WaveChannel::readRAM(u8 address) const
	{

		if (between(address, 0x30, 0x3F))
		{
			return _waveRAM[address - 0x30];
		}
		else
		{
			return 0xFF;
		}
	}

	/*
	 * Write to the DAC register (NR30).
	 */
	void WaveChannel::writeDAC(u8 newDAC)
	{

		_state._dac = bit(newDAC, 7);
		_state._active = _state._dac;
	}

	/*
	 * Read from the DAC register (NR30).
	 */
	u8 WaveChannel::readDAC() const
	{

		return 0x7F | ((u8)_state._dac << 6);
	}

	/*
	 * Write to the length register (NRx1).
	 */
	void WaveChannel::writeLength(u8 newLength)
	{

		_length_timer = newLength;
	}

	/*
	 * Write to the volume (NR32).
	 */
	void WaveChannel::writeVolume(u8 newVolume)
	{

		_output_level = (newVolume & 0x60) >> 5;
	}

	/*
	 * Read from the volume (NR32).
	 */
	u8 WaveChannel::readVolume() const
	{

		return 0x9F | ((u8)_output_level << 5);
	}

	/*
	 * Write to the period and the control registers (NRx3 and NRx4).
	 *
	 * todo: sound length is not yet implemented
	 */
	void WaveChannel::writePeriodControl(u8 selection, u8 newPeriodControl)
	{

		// NRx3
		if (selection == 0)
		{

			_freq = (_freq & 0x700) | newPeriodControl;
			_timer = (0x800 - _freq) * 2;
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

				_step = 0;
			}
		}
	}

	/*
	 * Read from the control register (NRx4).
	 */
	u8 WaveChannel::readControl() const
	{

		return _state._length;
	}

	/**
	 * Compute the current volume of the channel.
	 */
	void WaveChannel::addSample(float &leftSample, float &rightSample) const
	{

		if (_state._active && _state._dac && !_muted)
		{

			u8 samplePair = _waveRAM[_step / 2];

			u8 signal = ((_step % 2 == 0) ? hiNibble(samplePair) : loNibble(samplePair));

			u8 digitalSignal = signal >> (_output_level - 1) % 5; // check table

			float analogSignal = -(2 * digitalSignal / 16.0f) + 1;

			leftSample += analogSignal * _state._left;
			rightSample += analogSignal * _state._right;
		}
	}
}
