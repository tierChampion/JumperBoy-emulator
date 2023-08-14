#include <apu/apu.h>

namespace jmpr {

	/**
	* Generate a sample to later be queued for audio.
	*/
	void APU::generateSample() {

		float sample2 = generateChannel2();

		// add other channels

		float leftSample = _channels[1]._left * sample2;
		float rightSample = _channels[1]._right * sample2;

		leftSample *= _left_vol + 1;
		rightSample *= _right_vol + 1;

		_samples[_sample_pointer++] = leftSample;
		_samples[_sample_pointer++] = rightSample;

		if (_sample_pointer == MAX_SAMPLES) {

			// TODO: DISPATCH TO SDL

			_sample_pointer = 0;
		}
	}
}