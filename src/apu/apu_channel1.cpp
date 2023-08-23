#include <apu/apu.h>

namespace jmpr {

	// NR10 80 (wave duty of 00, initial length timer of 111111)
	// NR11 BF (wave duty of 00, initial length timer of 111111)
	// NR12 F3 (initial volume of 0, envelope decrease and no sweep. DAC off)
	// NR13 FF (low 8 bits of period is 11111111)
	// NR14 BF (trigger on, sound length disabled, upper bits of period is 111)

	void APU::initChannel1() {

		_channels[0] = AudioChannelState();
		_channels[0]._active = true;
		_channels[0]._dac = true;
		_channels[0]._left = true;
		_channels[0]._right = true;

		// todo...
	}

}