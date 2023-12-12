#include <apu/apu.h>

namespace jmpr
{
	/**
	 * Generate a sample to later be queued for audio.
	 */
	void APU::generateSample()
	{
		std::array<float, 2> sample = {0, 0};

		_channel1.addSample(sample[0], sample[1]);
		_channel2.addSample(sample[0], sample[1]);
		_channel3.addSample(sample[0], sample[1]);
		_channel4.addSample(sample[0], sample[1]);

		sample[0] *= (_left_vol + 1);
		sample[1] *= (_right_vol + 1);

		_samples[_sample_pointer++] = (sample[0] / (AUDIO_CHANNEL_COUNT * 8.0f)) * _main_volume;
		_samples[_sample_pointer++] = (sample[1] / (AUDIO_CHANNEL_COUNT * 8.0f)) * _main_volume;

		if (_sample_pointer % MAX_SAMPLES == 0)
		{
			_ready_buffer = _sample_pointer == MAX_SAMPLES ? 0 : 1;
			_sample_pointer = _ready_buffer == 0 ? _sample_pointer : 0;
		}
	}

	const float* APU::getSamples() const
	{
		return &_samples[_ready_buffer * MAX_SAMPLES];
	}
}