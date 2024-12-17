#pragma once

#include <apu/channel.h>

namespace jmpr {

	class WaveChannel : public AudioChannel {

		std::array<u8, 0x10> _waveRAM;

		u8 _output_level;

		u8 _step; 
		u16 _freq; 
		u16 _timer;

		u8 _addr_start;

	public:

		WaveChannel() {}
		WaveChannel(u8 addressSpaceStart);

		void reset() override;

		void update() override;
		void updateEnvelope() override {}

		void write(u8 address, u8 data) override;
		u8 read(u8 address) const override;

		void writeToRAM(u8 address, u8 newSamples);
		u8 readRAM(u8 address) const;

		virtual void addSample(float& leftSample, float& rightSample) const override;

	private:

		void writeDAC(u8 newDAC);
		u8 readDAC() const;

		void writeLength(u8 newLength);

		void writeVolume(u8 newVolume);
		u8 readVolume() const;

		void writePeriodControl(u8 selection, u8 newPeriodControl);
		u8 readControl() const;
	};
}
