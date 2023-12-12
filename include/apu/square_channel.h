#pragma once

#include <apu/channel.h>

namespace jmpr {

	class SquareChannel : public AudioChannel {

		u8 _duty;
		u8 _duty_step; 

		u8 _base_vol;
		bool _rising_envelope;
		u8 _envelope_pace;
		u8 _envelope_step;
		u8 _vol;

		u16 _freq; 
		u16 _timer;

		u8 _addr_start;

	public:

		SquareChannel() {}
		SquareChannel(bool activate, u8 addressSpaceStart);

		void reset() override;

		void update() override;

		void updateEnvelope() override;

		void write(u8 address, u8 data) override;
		u8 read(u8 address) const override;

		virtual void addSample(float& leftSample, float& rightSample) const override;

	private:

		void writeSweep(u8 newSweep);
		u8 readSweep() const;

		void writeLengths(u8 newLength);
		u8 readDuty() const;

		void writeVolume(u8 newVolume);
		u8 readVolume() const;

		void writePeriodControl(u8 selection, u8 newPeriodControl);
		u8 readControl() const;
	};
}
