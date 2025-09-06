#pragma once
#include <SDL.hpp>
#include "Datatypes/TimeStamp.h"
#include "Base/AudioDevice.h"

// Will remplace the actual Music and Sound classes

class Audio
{
    friend class AudioDevice;

    public:
    AudioData& Load(AudioData& data) { m_data =  &data; return data;}

    TimeStamp timePosition() { return m_samplepos / (TimeStamp)m_data->freq(); }
    TimeStamp timeLength() { return m_data->len() / (TimeStamp)m_data->freq(); }
    
    private:

    AudioData* m_data = nullptr;

    float m_fsamplepos = 0; // The position of the sample with subsample precition
    uint32_t m_samplepos = 0; // The position (in samples) of the audio sample
    uint32_t m_samplestartfade = 0; // The position (in samples) in which the sample starts to fade in
    uint32_t m_sampleendfade = 0; // The position in which the sample ends to fade in
    /*If m_sampleendfade is lower than m_samplestartfade then the audio fades out
    If that's the case, then when m_samplepos gets higher than m_samplestartfade, the audio will automatically stop*/
};