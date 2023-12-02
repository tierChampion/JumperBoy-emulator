#include <apu/apu.h>

namespace jmpr {

	APU::APU() 
	{
		// Initialize SDL Audio
		// if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		// 	std::cerr << "Couldn't initialize SDL Audio: " << SDL_GetError() << std::endl;
		// 	exit(-10);
		// }

		// _audio_specs = SDL_AudioSpec();
		// SDL_zero(_audio_specs);
		// _audio_specs.freq = SAMPLE_RATE;
		// _audio_specs.format = AUDIO_F32;
		// _audio_specs.channels = 2;
		// _audio_specs.samples = MAX_SAMPLES;
		// _audio_specs.callback = nullptr;

		// _audio_id = SDL_OpenAudioDevice(NULL, 0, &_audio_specs, nullptr, 0);

		// SDL_PauseAudioDevice(_audio_id, 0);

		_sample_pointer = 0;
		_sample_counter = 0;

		_apu_power = true;

		_left_vol = 0b111;
		_right_vol = 0b111;

		_channel1 = SquareChannel(true, 0x10);
		_channel2 = SquareChannel(false, 0x15);
		_channel3 = WaveChannel(0x1A);
		_channel4 = NoiseChannel(0x20);
	}

	APU::~APU() {

		// SDL_CloseAudioDevice(_audio_id);
	}

	void APU::reboot() {

		// todo
	}

	void APU::updateEffects() {

		_div_apu = (_div_apu + 1) % 8;

		if (_div_apu % 2 == 0) {
			// length
			_channel1.updateLengthTimer();
			_channel2.updateLengthTimer();
			_channel3.updateLengthTimer();
			_channel4.updateLengthTimer();

			if (_div_apu % 4 == 0) {
				// sweep

				if (_div_apu % 8 == 0) {
					// enveloppe
					_channel1.updateEnvelope();
					_channel2.updateEnvelope();
					_channel4.updateEnvelope();
				}
			}
		}
	}

	void APU::update() {

		_sample_counter++;
		_channel1.update();
		_channel2.update();
		_channel3.update();
		_channel4.update();

		// generate a sample
		if (_sample_counter * _sample_pointer >= SAMPLE_GATERING * _sample_pointer) {

			generateSample();
			_sample_counter = 0;
		}
	}

	u8 APU::read(u8 address) {

		if (between(address, 0x10, 0x15))
			return _channel1.read(address);
		else if (between(address, 0x16, 0x19))
			return _channel2.read(address);
		else if (between(address, 0x1A, 0x1F))
			return _channel3.read(address);
		else if (between(address, 0x20, 0x23))
			return _channel4.read(address);
		else if (address == 0x24)
			return getMasterVolume();
		else if (address == 0x25)
			return getPanning();
		else if (address == 0x26)
			return getAPUPower();
		else if (between(address, 0x30, 0x3F))
			return _channel3.readRAM(address);

		return 0xFF;
	}

	void APU::write(u8 address, u8 data) {

		if (between(address, 0x10, 0x15))
			_channel1.write(address, data);
		if (between(address, 0x16, 0x19))
			_channel2.write(address, data);
		if (between(address, 0x1A, 0x1F))
			_channel3.write(address, data);
		if (between(address, 0x20, 0x23))
			_channel4.write(address, data);
		else if (address == 0x24)
			updateMasterVolume(data);
		else if (address == 0x25)
			updateChannelPanning(data);
		else if (address == 0x26)
			updateAPUPower(data);
		else if (between(address, 0x30, 0x3F))
			_channel3.writeToRAM(address, data);
	}

	// NR52

	u8 APU::getAPUPower() const {

		u8 result = (_apu_power << 7) | 0x70;

		result |= _channel1.isActive();
		result |= _channel2.isActive() << 1;
		result |= _channel3.isActive() << 2;
		result |= _channel4.isActive() << 3;

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

	u8 APU::getPanning() const {

		u8 result = 0;

		result |= (_channel1.outputsLeft() << AUDIO_CHANNEL_COUNT) | (_channel2.outputsRight());
		result |= (_channel2.outputsLeft() << AUDIO_CHANNEL_COUNT + 1) | (_channel2.outputsRight() << 1);
		result |= (_channel3.outputsLeft() << AUDIO_CHANNEL_COUNT + 2) | (_channel3.outputsRight() << 2);
		result |= (_channel4.outputsLeft() << AUDIO_CHANNEL_COUNT + 3) | (_channel4.outputsRight() << 3);

		return result;
	}

	void APU::updateChannelPanning(u8 newPanning) {

		_channel1.updatePanning(bit(newPanning, AUDIO_CHANNEL_COUNT), bit(newPanning, 0));
		_channel2.updatePanning(bit(newPanning, AUDIO_CHANNEL_COUNT + 1), bit(newPanning, 1));
		_channel3.updatePanning(bit(newPanning, AUDIO_CHANNEL_COUNT + 2), bit(newPanning, 2));
		_channel4.updatePanning(bit(newPanning, AUDIO_CHANNEL_COUNT + 3), bit(newPanning, 3));
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