#pragma once

#include <common.h>

#include <array>

namespace jmpr
{
	struct AudioChannelState
	{
		bool _active;
		bool _dac;
		bool _length;
		bool _left;
		bool _right;
	};

	class AudioChannel
	{
	protected:
		AudioChannelState _state;

		u8 _length_timer;
		bool _muted;

	public:
		AudioChannel();

		virtual void reset() = 0;

		virtual void update() = 0;
		void updateLengthTimer();

		virtual void updateEnvelope() = 0;

		virtual void write(u8 address, u8 data) = 0;
		virtual u8 read(u8 address) const = 0;

		virtual void addSample(float& leftSample, float& rightSample) const = 0;

		void updatePanning(bool newLeft, bool newRight);

		bool isActive() const { return _state._active; }
		bool outputsLeft() const { return _state._left; }
		bool outputsRight() const { return _state._right; }
		void toggleMuted() { _muted = !_muted; }
	};
}
