#pragma once

#include <common.h>

#include <array>

namespace jmpr {

	struct AudioChannelState {

		bool _active;
		bool _dac;
		bool _left;
		bool _right;
	};

	class AudioChannel {

	protected:

		AudioChannelState _state;

	public:

		AudioChannel();

		virtual void reset() = 0;

		virtual void update() = 0;

		virtual void write(u8 address, u8 data) = 0;
		virtual u8 read(u8 address) const = 0;

		virtual std::array<float, 2> sample() = 0;

		void updatePanning(bool newLeft, bool newRight);

		bool isActive() const { return _state._active; }
		bool outputsLeft() const { return _state._left; }
		bool outputsRight() const { return _state._right; }
	};
}
