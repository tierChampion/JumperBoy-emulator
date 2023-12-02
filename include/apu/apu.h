#pragma once

#include <common.h>

#include <apu/square_channel.h>
#include <apu/wave_channel.h>
#include <apu/noise_channel.h>

#include <SDL2/SDL.h>
#include <vector>

namespace jmpr
{

#define AUDIO_CHANNEL_COUNT 4

	class APU
	{
	public:
		static const u16 SAMPLE_RATE = 44100;
		static const u8 SAMPLE_GATERING = 98;
		static const u16 MAX_SAMPLES = 4096;

	private:
		SDL_AudioDeviceID _audio_id;

		float _samples[APU::MAX_SAMPLES];
		u16 _sample_pointer;
		u8 _sample_counter;

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

		void setAudioId(SDL_AudioDeviceID id) { _audio_id = id; }

		void reboot();

		u8 read(u8 address);
		void write(u8 address, u8 data);

		void updateEffects();
		void update();

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
	};
}