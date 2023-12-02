#include <ui/audio_master.h>

namespace jmpr
{

    const float DEFAULT_VOLUME = 0.05f;

    AudioMaster::AudioMaster(APU *apu)
    {
        // Initialize SDL Audio
        if (SDL_Init(SDL_INIT_AUDIO) < 0)
        {
            std::cerr << "Couldn't initialize SDL Audio: " << SDL_GetError() << std::endl;
            exit(-10);
        }

        _audio_specs = SDL_AudioSpec();
        SDL_zero(_audio_specs);
        _audio_specs.freq = APU::SAMPLE_RATE;
        _audio_specs.format = AUDIO_F32;
        _audio_specs.channels = 2;
        _audio_specs.samples = APU::MAX_SAMPLES / 2;
        _audio_specs.callback = nullptr;
        _audio_specs.userdata = nullptr;

        _audio_id = SDL_OpenAudioDevice(NULL, 0, &_audio_specs, nullptr, 0);

        apu->setAudioId(_audio_id);

        SDL_PauseAudioDevice(_audio_id, 0);
    }

    AudioMaster::~AudioMaster()
    {
        SDL_CloseAudioDevice(_audio_id);
    }
}
