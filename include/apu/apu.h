#pragma once

#include <common.h>

namespace jmpr {

	class APU {

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

	public:

		APU();

		u8 read(u8 address);
		void write(u8 address, u8 data);
	};

}