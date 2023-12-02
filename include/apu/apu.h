#pragma once

#include <common.h>

#include <apu/square_channel.h>
#include <apu/wave_channel.h>
#include <apu/noise_channel.h>

#include <SDL2/SDL.h>
#include <vector>

namespace jmpr {

#define AUDIO_CHANNEL_COUNT 4

	// desired sample rate of output (48000)
#define SAMPLE_RATE 44100 
	// every about 87 t-states, collect one audio sample (87.3813...)
#define SAMPLE_GATERING 95.10893424f // but empty the queue before queuing more
	// max number of samples kept by the apu before being flushed to sdl
#define MAX_SAMPLES 4096

	class APU {

		SDL_AudioDeviceID _audio_id;
		SDL_AudioSpec _audio_specs;

		float _samples[2 * MAX_SAMPLES];
		u16 _sample_pointer;
		u8 _sample_counter;
		u8 _extra_counter;

		bool _apu_power;
		u8 _div_apu;

		u8 _left_vol;
		u8 _right_vol;

		SquareChannel _channel1;
		SquareChannel _channel2;
		WaveChannel _channel3;
		NoiseChannel _channel4;

	public:

		APU();
		~APU();

		void reboot();

		u8 read(u8 address);
		void write(u8 address, u8 data);

		void updateEffects();

		void update();

		void generateSample();

		void dispatchSamples(uint8_t* stream, int len);

	private:

		// NR52
		void updateAPUPower(u8 newPower);
		u8 getAPUPower() const;

		// NR51
		void updateChannelPanning(u8 newPanning);
		u8 getPanning() const;

		// NR50
		void updateMasterVolume(u8 newVol);
		u8 getMasterVolume() const;
	};
}