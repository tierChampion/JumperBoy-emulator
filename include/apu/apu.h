#pragma once

#include <common.h>

#include <apu/square_channel.h>

#include <SDL/SDL.h>

namespace jmpr {

#define AUDIO_CHANNEL_COUNT 4

	// desired sample rate of output
#define SAMPLE_RATE 48000 
	// every about 87 t-states, collect one audio sample
#define SAMPLE_GATERING 87.38133333333333f 
	// max number of samples kept by the apu before being flushed to sdl
#define MAX_SAMPLES 2048

	class APU {

		// sdl stuff
		SDL_AudioDeviceID _audio_id;
		SDL_AudioSpec _audio_specs;

		float _samples[MAX_SAMPLES];
		u16 _sample_pointer;
		u8 _sample_counter;

		bool _apu_power;
		u8 _div_apu;

		u8 _left_vol;
		u8 _right_vol;

		AudioChannelState _channels[AUDIO_CHANNEL_COUNT];

		// CH2
		SquareChannel _channel2;

		u8 _waveRAM[0x10];


	public:

		APU();
		~APU();

		void reboot();

		u8 read(u8 address);
		void write(u8 address, u8 data);

		// div apu updates FIX LATER TO RREMOVE EDGES AND KEEP IT IN THE TIMER.
		void updateEffects(bool fallingEdge);

		// regular timer updates
		void updateChannels();

		void generateSample();

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

		// CH1
		void initChannel1();

		// CH3
		void initChannel3();

		// CH4
		void initChannel4();



		void test();
	};
}