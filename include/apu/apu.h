#pragma once

#include <common.h>

namespace jmpr {

#define AUDIO_CHANNEL_COUNT 4

	struct AudioChannel {

		bool _gen_power;

		u8 _panning;

		u8 _len_timer;
		u8 _timer_begin;
		u8 _timer_end;
		bool _len_enabled;

	public:

		void update();
	};

	class APU {

		bool _apu_power;

		u8 _div_apu;

		AudioChannel _channels[AUDIO_CHANNEL_COUNT];

		u8 _left_vol;
		u8 _right_vol;

		u8 _ch2_lengths;
		u8 _ch2_vol;
		u8 _ch2_wavelength_lo;
		u8 _ch2_wavelength_hi;

		u8 _waveRAM[0x10];

	public:

		APU();

		u8 read(u8 address);
		void write(u8 address, u8 data);

		void update(u8 oldEdge, u8 newEdge);

		void mixChannels(u8* stream, int len);

		// Generators
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