#include <apu/channel.h>

namespace jmpr {

	AudioChannel::AudioChannel() {

		_state = AudioChannelState();
		_state._active = false;
		_state._dac = false;
		_state._left = true;
		_state._right = true;
	}

	void AudioChannel::updatePanning(bool newLeft, bool newRight) {

		_state._left = newLeft;
		_state._right = newRight;
	}
}
