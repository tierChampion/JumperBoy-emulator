#pragma once

#include <apu/apu.h>

namespace jmpr
{
    class AudioMaster
    {
        SDL_AudioDeviceID _audio_id;
        SDL_AudioSpec _audio_specs;
        APU *_apu;

    public:
        AudioMaster(APU *apu);
        ~AudioMaster();

        void audioMasterCallback(u8 *stream, int len);
        static void audioCallback(void *userData, u8 *stream, int len);
    };
}
