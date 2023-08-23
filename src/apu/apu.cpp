#include <apu/apu.h>

namespace jmpr {

	// NR50 77 (volume of 7 in both outputs)
	// NR51 F3 (all channels in both, except channel 4 only in left output)
	// NR52 F1 (apu on, channel 1 on)

	APU::APU() : _waveRAM{ 0 } {

		// Initialize SDL Audio
		if (SDL_Init(SDL_INIT_AUDIO) < 0) {
			std::cerr << "Couldn't initialize SDL Audio: " << SDL_GetError() << std::endl;
			exit(-10);
		}

		_audio_specs = SDL_AudioSpec();
		SDL_zero(_audio_specs);
		_audio_specs.freq = SAMPLE_RATE;
		_audio_specs.format = AUDIO_F32;
		_audio_specs.channels = 2;
		_audio_specs.samples = MAX_SAMPLES;
		_audio_specs.callback = nullptr;

		_audio_id = SDL_OpenAudioDevice(NULL, 0, &_audio_specs, nullptr, 0);

		SDL_PauseAudioDevice(_audio_id, 0);

		_sample_pointer = 0;
		_sample_counter = 0;

		_apu_power = true;

		_left_vol = 0b111;
		_right_vol = 0b111;

		initChannel1();

		_channel2 = SquareChannel();

		initChannel3();
		initChannel4();
	}

	APU::~APU() {

		SDL_CloseAudioDevice(_audio_id);
	}

	void APU::reboot() {

		// todo
	}

	void APU::updateEffects(bool fallingEdge) {

		if (fallingEdge) {

			_div_apu++;

			// every 2, sound length
			if ((_div_apu & 0b1) == 0) {

			}

			// every 4, ch1 freq sweep
			if ((_div_apu & 0b11) == 0) {

			}

			// every 8, envelope sweep
			if ((_div_apu & 0b111) == 0) {

			}
		}
	}

	void APU::updateChannels() {

		// every m cycle

		_sample_counter++;
		_channel2.update();

		/*/
		_ch2_timer--;

		if (_ch2_timer == 0) {

			_ch2_duty_pointer = (_ch2_duty_pointer + 1) % 8;
			_ch2_timer = (0x800 - _ch2_freq) * 4;
		}
		*/

		// generate a sample
		if (_sample_counter * _sample_pointer >= SAMPLE_GATERING * _sample_pointer) {

			generateSample();
			_sample_counter = 0;
		}

	}

	// TODO, link read write to channel2

	u8 APU::read(u8 address) {

		if (between(address, 0x16, 0x19))
			return _channel2.read(address);// readChannel2(address);
		else if (address == 0x24)
			return getMasterVolume();
		else if (address == 0x25)
			return getPanning();
		else if (address == 0x26)
			return getAPUPower();
		else if (between(address, 0x30, 0x3F))
			return _waveRAM[address - 0x30];

		return 0xFF;
	}

	void APU::write(u8 address, u8 data) {

		if (between(address, 0x16, 0x19))
			_channel2.write(address, data);// writeChannel2(address, data);
		else if (address == 0x24)
			updateMasterVolume(data);
		else if (address == 0x25)
			updateChannelPanning(data);
		else if (address == 0x26)
			updateAPUPower(data);
		else if (between(address, 0x30, 0x3F))
			_waveRAM[address - 0x30] = data;
	}

	// NR52

	// TODO change for object channels
	u8 APU::getAPUPower() const {

		u8 result = (_apu_power << 7) | 0b01110000;

		for (u8 c = 0; c < AUDIO_CHANNEL_COUNT; c++) {
			if (c != 1) {
				result |= _channels[c]._active << c;
			}
			else {
				result |= _channel2.isActive() << 2;
			}
		}

		return result;
	}

	void APU::updateAPUPower(u8 newPower) {

		u8 powerStatus = bit(newPower, 7);

		_apu_power = (powerStatus > 0);

		// Power OFF
		if (!_apu_power) {

			// todo: set every single registers in the apu to 0.
		}
	}

	// NR51

	// TODO change for channels
	u8 APU::getPanning() const {

		u8 result = 0;

		for (u8 c = 0; c < AUDIO_CHANNEL_COUNT; c++) {

			if (c != 1) {
				result |= (_channels[c]._left << (AUDIO_CHANNEL_COUNT + c)) | (_channels[c]._right << c);
			}
			else {
				result |= (_channel2.outputsLeft() << 5) | (_channel2.outputsRight() << 1);
			}
		}

		return result;
	}

	void APU::updateChannelPanning(u8 newPanning) {

		for (u8 c = 0; c < AUDIO_CHANNEL_COUNT; c++) {

			if (c != 1) {
				_channels[c]._left = bit(newPanning, AUDIO_CHANNEL_COUNT + c) << 1;
				_channels[c]._right = bit(newPanning, c);
			}
			else {
				_channel2.updatePanning(bit(newPanning, AUDIO_CHANNEL_COUNT + c), bit(newPanning, c));
			}
		}
	}

	// NR50

	void APU::updateMasterVolume(u8 newVol) {

		_left_vol = (newVol & 0b01110000) >> 4;
		_right_vol = (newVol & 0b00000111);
	}

	u8 APU::getMasterVolume() const {

		u8 result = 0;

		result |= _left_vol << 4;
		result |= _right_vol;

		return result;
	}
}