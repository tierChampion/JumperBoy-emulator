#pragma once

#include <apu/channel.h>

namespace jmpr {

	class SquareChannel : public AudioChannel {

		u8 _duty;
		u8 _duty_step; // where in the cycle you are. increments everytime the period timer goes to 0
		u8 _vol;
		u16 _freq; // length of a single step of duty cycles. decrements every m_cycle
		u16 _timer;

		std::array<float, 2> _out;

	public:

		SquareChannel();

		void reset() override;

		void update() override;

		void write(u8 address, u8 data) override;
		u8 read(u8 address) const override;

		std::array<float, 2> sample() override;

	private:

		void writeChannel2Lengths(u8 newLength);
		u8 readChannel2Duty() const;

		void writeChannel2Volume(u8 newVolume);
		u8 readChannel2Volume() const;

		void writeChannel2PeriodControl(u8 selection, u8 newPeriodControl);
		u8 readChannel2Control() const;
	};
}
