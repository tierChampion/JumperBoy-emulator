#pragma once

#include <common.h>

#include <apu/square_channel.h>
#include <apu/wave_channel.h>
#include <apu/noise_channel.h>

#include <SDL2/SDL.h>
#include <vector>

namespace jmpr
{
	class APU
	{
	public:
		static const u8 AUDIO_CHANNEL_COUNT = 4;
		static const u8 SAMPLE_GATERING = GB_CLOCK_SPEED / AUDIO_SAMPLE_RATE;
		static const u16 MAX_SAMPLES = 2048;

	private:
		SDL_AudioDeviceID _audio_id;

		float _samples[APU::MAX_SAMPLES];
		u16 _sample_pointer;
		u8 _sample_counter;

		bool _apu_power;
		u8 _div_apu;

		u8 _left_vol;
		u8 _right_vol;
		float _main_volume;

		SquareChannel _channel1;
		SquareChannel _channel2;
		WaveChannel _channel3;
		NoiseChannel _channel4;

	public:
		APU();

		void setAudioId(SDL_AudioDeviceID id) { _audio_id = id; }
		void setVolume(float volume) { _main_volume = volume; }
		void toggleChannel(u8 channelId);

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