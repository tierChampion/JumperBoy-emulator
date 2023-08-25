#include <apu/channel.h>

namespace jmpr {

	AudioChannel::AudioChannel() {

		_state = AudioChannelState();
		_state._active = false;
		_state._dac = false;
		_state._length = false;
		_state._left = true;
		_state._right = true;

		_out[0] = 0;
		_out[1] = 0;

		_length_timer = 0b111111;
	}

	void AudioChannel::updateLengthTimer() {

		if (_state._length) {

			_length_timer++;

			if (_length_timer >= 64) {

				_state._active = false;
			}
		}
	}

	void AudioChannel::updatePanning(bool newLeft, bool newRight) {

		_state._left = newLeft;
		_state._right = newRight;
	}
}
