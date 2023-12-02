#include <apu/apu.h>

namespace jmpr
{
	/**
	 * Generate a sample to later be queued for audio.
	 */
	void APU::generateSample()
	{
		std::array<float, 2> sample;

		std::array<float, 2> sample1 = _channel1.sample();
		std::array<float, 2> sample2 = _channel2.sample();
		std::array<float, 2> sample3 = _channel3.sample();
		std::array<float, 2> sample4 = _channel4.sample();

		sample[0] = (sample1[0] + sample2[0] + sample3[0] + sample4[0]) * (_left_vol + 1);
		sample[1] = (sample1[1] + sample2[1] + sample3[1] + sample4[1]) * (_right_vol + 1);

#define GLOBAL_VOLUME 0.05f

		_samples[_sample_pointer++] = (sample[0] / (AUDIO_CHANNEL_COUNT * 8.0f)) * GLOBAL_VOLUME;
		_samples[_sample_pointer++] = (sample[1] / (AUDIO_CHANNEL_COUNT * 8.0f)) * GLOBAL_VOLUME;

		if (_sample_pointer % MAX_SAMPLES == 0)
		{
			// 	SDL_QueueAudio(_audio_id, _samples, MAX_SAMPLES * sizeof(float));
			_sample_pointer %= 2 * MAX_SAMPLES;
		}
	}

	void APU::dispatchSamples(uint8_t *stream, int len)
	{
		float *floatStream = reinterpret_cast<float *>(stream);

		// sometimes sounds weird, find edge cases were it queues unfinished samples

		uint16_t start = _sample_pointer >= MAX_SAMPLES ? 0 : MAX_SAMPLES;
		std::cout << static_cast<int>(_sample_pointer) << std::endl;

		for (uint16_t i = 0; i < (len / 4); i++)
		{
			floatStream[i] = _samples[i + start];
		}

		_extra_counter = 0;
	}
}