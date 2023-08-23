#include <apu/apu.h>

namespace jmpr {

	/**
	* Generate a sample to later be queued for audio.
	*/
	void APU::generateSample() {

		std::array<float, 2> sample2 = _channel2.sample();
		// add other channels...

		sample2[0] *= _left_vol + 1;
		sample2[1] *= _right_vol + 1;

#define GLOBAL_VOLUME 0.3f

		_samples[_sample_pointer++] = (sample2[0] / 8.0f) * GLOBAL_VOLUME;
		_samples[_sample_pointer++] = (sample2[1] / 8.0f) * GLOBAL_VOLUME;

		if (_sample_pointer == MAX_SAMPLES) {

			SDL_QueueAudio(_audio_id, _samples, MAX_SAMPLES * sizeof(float));

			_sample_pointer = 0;
		}
	}
}