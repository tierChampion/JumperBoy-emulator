#pragma once

#include <common.h>

namespace jmpr {

#define AUDIO_CHANNEL_COUNT 4

	// desired sample rate of output
#define SAMPLE_RATE 44100 // 48000
	// every 95 t-states, collect one audio sample
#define SAMPLE_GATERING 96 // 88
	// max number of samples kept by the apu before being flushed to sdl
#define MAX_SAMPLES 1024

	struct AudioChannelState {

		bool _active;
		bool _dac;
		bool _left;
		bool _right;
	};

	class APU {

		bool _apu_power;

		u8 _samples[2 * MAX_SAMPLES];
		u16 _sample_pointer;

		u8 _div_apu;

		AudioChannelState _channels[AUDIO_CHANNEL_COUNT];

		u8 _left_vol;
		u8 _right_vol;

		// CH2
		u8 _ch2_duty;
		u8 _ch2_duty_pointer; // where in the cycle you are. increments everytime the period timer goes to 0
		u8 _ch2_vol;
		u16 _ch2_period_timer; // length of a single step of duty cycles. decrements every m_cycle
		u16 _ch2_period_pointer;

		u8 _waveRAM[0x10];


	public:

		APU();

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

		// CH2

		void initChannel2();

		void writeChannel2(u8 address, u8 data);
		u8 readChannel2(u8 address);

		void writeChannel2Lengths(u8 newLength);
		u8 readChannel2Duty() const;
		void writeChannel2Volume(u8 newVolume);
		u8 readChannel2Volume() const;
		void writeChannel2PeriodControl(u8 selection, u8 newPeriodControl);
		u8 readChannel2Control() const;

		float generateChannel2() const;
	};
}