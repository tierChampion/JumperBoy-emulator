#pragma once

#include <apu/apu.h>

namespace jmpr
{
    class AudioMaster
    {
        SDL_AudioDeviceID _audio_id;
        SDL_AudioSpec _audio_specs;

    public:
        AudioMaster(APU* apu);
        ~AudioMaster();
    };
}
