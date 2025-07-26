#pragma once
#include "Music.h"

class Sound: public Music
{
    public:
    Sound();
    ~Sound();

    void Play() override;
    void Stop() override;

    private:
    void LoadUserData(void* temp_rw) override;

    int channel_playing = -1;
};