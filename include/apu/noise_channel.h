#pragma once

#include <apu/channel.h>

namespace jmpr {

	// 100 0000 0000 0000 0000 0000 (4194304)
	// 100 0000 0000 0000 0000 (262144)
	// denom: divider * 2^shift (if divider == 0 -> divider = 0.5)

	// timer = (div << shift + 4) *** if div != 0 ***
	// timer = (1 << shift + 3) *** if div == 0 ***

	class NoiseChannel : public AudioChannel {

		u16 _lfsr;
		u8 _clk_shift;
		u8 _lfsr_size;
		u8 _clk_divider;
		u16 _timer;

		u8 _base_vol;
		bool _rising_envelope;
		u8 _envelope_pace;
		u8 _envelope_step;
		u8 _vol;

		u8 _addr_start;

	public:

		NoiseChannel() {}
		NoiseChannel(u8 addressSpaceStart);

		void reset() override;

		void update() override;

		void updateEnvelope() override;

		void write(u8 address, u8 data) override;
		u8 read(u8 address) const override;

		virtual void addSample(float& leftSample, float& rightSample) const override;

	private:

		void writeLength(u8 newLength);

		void writeVolume(u8 newVolume);
		u8 readVolume() const;

		void writeFrequency(u8 newSound);
		u8 readFrequency() const;

		void writeControl(u8 newControl);
		u8 readControl() const;
	};
}
