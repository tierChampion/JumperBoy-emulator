#include <ui/audio_manager.h>

#include <gb.h>

namespace jmpr
{
    AudioManager::AudioManager(APU *apu)
        : _apu(apu)
    {
        init();
    }

    void AudioManager::init()
    {
        SDL_AudioSpec specs = SDL_AudioSpec();
        SDL_zero(specs);
        specs.freq = AUDIO_SAMPLE_RATE;
        specs.format = AUDIO_F32;
        specs.channels = 2;
        specs.samples = APU::MAX_SAMPLES / 2;
        specs.callback = &AudioManager::staticCallback;
        specs.userdata = this;

        _audio_id = SDL_OpenAudioDevice(NULL, 0, &specs, nullptr, 0);

        SDL_PauseAudioDevice(_audio_id, 0);
    }

    void AudioManager::audioCallback(float *stream, int len)
    {
        if (GameBoy::isRunning())
        {
            bool bufferOk = true;

            const float* buffer = _apu->getSamples();

            for (int i = 0; i < len; i++)
            {
                stream[i] = buffer[i];
            }
        }
        else
        {
            for (int i = 0; i < len; i++)
            {
                stream[i] = 0;
            }
        }
    }
}