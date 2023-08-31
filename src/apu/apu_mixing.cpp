#include <apu/apu.h>

namespace jmpr {

	/**
	* Generate a sample to later be queued for audio.
	*/
	void APU::generateSample() {

		std::array<float, 2> sample;

		std::array<float, 2> sample1 = _channel1.sample();
		std::array<float, 2> sample2 = _channel2.sample();
		std::array<float, 2> sample4 = _channel4.sample();
		// add other channels...

		sample[0] = (sample4[0]) * (_left_vol + 1); // sample1[0] + sample2[0] + 
		sample[1] = (sample4[1]) * (_right_vol + 1); // sample1[1] + sample2[1] + 

#define GLOBAL_VOLUME 0.05f

		_samples[_sample_pointer++] = (sample[0] / (8.0f)) * GLOBAL_VOLUME;
		_samples[_sample_pointer++] = (sample[1] / (8.0f)) * GLOBAL_VOLUME;

		if (_sample_pointer == MAX_SAMPLES) {

			SDL_QueueAudio(_audio_id, _samples, MAX_SAMPLES * sizeof(float));

			_sample_pointer = 0;
		}
	}
}