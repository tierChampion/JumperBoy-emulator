#include <ui/audio_master.h>

namespace jmpr
{

    AudioMaster::AudioMaster(APU *apu)
        : _apu(apu)
    {
        // Initialize SDL Audio
        if (SDL_Init(SDL_INIT_AUDIO) < 0)
        {
            std::cerr << "Couldn't initialize SDL Audio: " << SDL_GetError() << std::endl;
            exit(-10);
        }

        _audio_specs = SDL_AudioSpec();
        SDL_zero(_audio_specs);
        _audio_specs.freq = SAMPLE_RATE;
        _audio_specs.format = AUDIO_F32;
        _audio_specs.channels = 2;
        _audio_specs.samples = MAX_SAMPLES / 2;
        _audio_specs.callback = &audioCallback;
        _audio_specs.userdata = this;

        _audio_id = SDL_OpenAudioDevice(NULL, 0, &_audio_specs, nullptr, 0);

        SDL_PauseAudioDevice(_audio_id, 0);
    }

    AudioMaster::~AudioMaster()
    {
        SDL_CloseAudioDevice(_audio_id);
    }

    void AudioMaster::audioMasterCallback(u8 *stream, int len)
    {
        _apu->dispatchSamples(stream, len);
    }

    void AudioMaster::audioCallback(void *userData, u8 *stream, int len)
    {
        const auto audioMaster = reinterpret_cast<AudioMaster *>(userData);
        audioMaster->audioMasterCallback(stream, len);
    }
}
