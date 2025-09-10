#pragma once
#include <SDL.hpp>
#include "Datatypes/TimeStamp.h"
#include "Base/AudioDevice.h"

// Will remplace the actual Music and Sound classes

struct AudioInfo
{
    int loop_start = 0, loop_end = -1;
    bool looped = false;

    float fade_in = 0, fade_out = 0;
    float speed = 1;
};

class Audio
{
    friend class AudioDevice;

    public:
    AudioData& Attach(AudioData& data) { m_data =  &data; return data;}

    TimeStamp timePosition() { return m_samplepos / (TimeStamp)m_data->freq(); }
    TimeStamp timeLength() { return m_data->len() / (TimeStamp)m_data->freq(); }

    AudioInfo Info;
    
    private:

    AudioData* m_data = nullptr;

    double m_fsamplepos = 0; // The position of the sample with subsample precition
    uint32_t m_samplepos = 0; // The position (in samples) of the audio sample

};