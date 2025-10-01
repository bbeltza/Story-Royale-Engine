#pragma once
#include <SDL.hpp>
#include "Datatypes/TimeStamp.h"
#include "Base/AudioDevice.hpp"

struct AudioInfo
{
    int loop_start = 0, loop_end = -1;
    bool looped = false;

    float fade_in = 0, fade_out = 0;
    float speed = 1;
    float volume = 1;
};

class Audio
{
    friend class AudDevice;

public:
    AudioInfo Info;

public:
    AudioData &Attach(AudioData &data)
    {
        m_data = &data;
        return data;
    }

    TimeStamp timePosition() { return m_samplepos / (TimeStamp)m_data->freq(); }
    TimeStamp timeLength() { return m_data->len() / (TimeStamp)m_data->freq(); }

    void FadeOut() { m_fadeout = true; }

private:
    AudioData *m_data = nullptr;

    double m_fsamplepos = 0;  // The position of the sample with subsample precition
    uint32_t m_samplepos = 0; // The position (in samples) of the audio sample

    bool m_fadein = false, m_fadeout = false;
    float m_fadevol = 1;
};