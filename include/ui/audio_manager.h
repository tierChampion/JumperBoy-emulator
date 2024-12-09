#pragma once

#include <apu/apu.h>

namespace jmpr
{
    class AudioManager
    {
        APU *_apu;
        SDL_AudioDeviceID _audio_id;

    public:
        AudioManager() {}
        AudioManager(APU *apu);

        void mute();
        void play();

    private:
        void init();

        void audioCallback(float *stream, int len);
        static void staticCallback(void *userdata, u8 *stream, int len)
        {
            AudioManager *player = static_cast<AudioManager *>(userdata);
            player->audioCallback(reinterpret_cast<float *>(stream), len / sizeof(float));
        }
    };
}