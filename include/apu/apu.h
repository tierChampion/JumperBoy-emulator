#pragma once

#include <common.h>

namespace jmpr {

#define AUDIO_CHANNEL_COUNT 4

	struct ApuRegisters {

		// Channel 1: Pulse with sweep
		u8 _NR10;		// Sweep
		u8 _NR11;		// Length and duty cycle
		u8 _NR12;		// Volume and enveloppe
		u8 _NR13;		// Wavelength low
		u8 _NR14;		// Wavelength high and control

		// Channel 2: Pulse
		u8 _NR21;		// Length and duty cycle
		u8 _NR22;		// Volume and enveloppe
		u8 _NR23;		// Wavelength low
		u8 _NR24;		// Wavelength high and control

		// Channel 3: Wave
		u8 _NR30;		// DAC
		u8 _NR31;		// Length
		u8 _NR32;		// Volume
		u8 _NR33;		// Wavelength low
		u8 _NR34;		// Wavelength high and control
		u8 _waveRAM[0x10];

		// Channel 4: Noise
		u8 _NR41;		// Length
		u8 _NR42;		// Volume and enveloppe
		u8 _NR43;		// Frequency and randomness
		u8 _NR44;		// Control

		// Global registers
		u8 _NR50;		// Master volume and VIN panning
		u8 _NR51;		// Sound panning
		u8 _NR52;		// Sound on/off

	};

	struct AudioChannel {

		bool _gen_power;

		u8 _panning;
	};

	class APU {

		ApuRegisters _registers;

		bool _apu_power;

		AudioChannel _channels[AUDIO_CHANNEL_COUNT];

		u8 _left_vol;
		u8 _right_vol;

		u8 _waveRAM[0x10];

	public:

		APU();

		u8 read(u8 address);
		void write(u8 address, u8 data);

		void mixChannels(u8* stream, int len);

		void channel1Generator(u8* stream, int len);
		void channel2Generator(u8* stream, int len);
		void channel3Generator(u8* stream, int len);
		void channel4Generator(u8* stream, int len);

	private:

		// NR52
		void handleAPUPower(u8 newPower);
		u8 getAPUPower() const;

		// NR51
		void updateChannelPanning(u8 newPanning);
		u8 getPanning() const;

		// NR50
		void updateMasterVolume(u8 newVol);
		u8 getMasterVolume() const;
	};

}