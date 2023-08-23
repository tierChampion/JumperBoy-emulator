#include <apu/apu.h>

namespace jmpr {

	// NR30 7F (wave duty of 00, initial length timer of 111111)
	// NR31 FF (wave duty of 00, initial length timer of 111111)
	// NR32 9F (initial volume of 0, envelope decrease and no sweep. DAC off)
	// NR33 FF (low 8 bits of period is 11111111)
	// NR34 BF (trigger on, sound length disabled, upper bits of period is 111)

	void APU::initChannel3() {

		_channels[2] = AudioChannelState();
		_channels[2]._active = false;
		_channels[2]._dac = false;
		_channels[2]._left = true;
		_channels[2]._right = true;

		// todo...
	}

}