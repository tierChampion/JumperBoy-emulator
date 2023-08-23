#include <apu/apu.h>

namespace jmpr {

	// NR41 FF (wave duty of 00, initial length timer of 111111)
	// NR42 00 (initial volume of 0, envelope decrease and no sweep. DAC off)
	// NR43 00 (low 8 bits of period is 11111111)
	// NR44 BF (trigger on, sound length disabled, upper bits of period is 111)

	void APU::initChannel4() {

		_channels[3] = AudioChannelState();
		_channels[3]._active = false;
		_channels[3]._dac = false;
		_channels[3]._left = true;
		_channels[3]._right = false;
	}

}