#include <ui/user_interface.h>

#include <apu/apu.h>

namespace jmpr
{

    void UI::initAudio(APU* apu)
    {
        SDL_AudioSpec specs = SDL_AudioSpec();
        SDL_zero(specs);
        specs.freq = APU::SAMPLE_RATE;
        specs.format = AUDIO_F32;
        specs.channels = 2;
        specs.samples = APU::MAX_SAMPLES / 2;
        specs.callback = nullptr;
        specs.userdata = nullptr;

        _audio_id = SDL_OpenAudioDevice(NULL, 0, &specs, nullptr, 0);

        apu->setAudioId(_audio_id);

        SDL_PauseAudioDevice(_audio_id, 0);
    }
}
