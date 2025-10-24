#include <SDL.hpp>
#include "Engine.hpp"
#include "Base/Audio.hpp"

#include "Classes/Thread.hpp"

#include "vorbis.h"

#include "utils.h"

void AudioData::Load()
{
    const int f_size = (int)m_file.getSize();
    const void* f_data = m_file.getRawData();

    SDL_RWops *audio_rw = SDL_RWFromConstMem(f_data, f_size);
    
    int channels_int;
    Uint8 *stream_data;
    SDL_zero(m_spec);
    
    if (SDL_LoadWAV_RW(audio_rw, 0, &m_spec, &stream_data, &m_len))
    {
        m_file.setType(File::T_WAV);
        m_data = (int8_t *)operator new(m_len);
        memcpy(m_data, stream_data, m_len);
        SDL_FreeWAV(stream_data);
        m_len /= AUDIO_BYTESIZE(m_spec.format);
        m_len /= m_spec.channels;
    }
    else if ((m_len = stb_vorbis_decode_memory((uint8_t *)f_data, f_size, &channels_int, &m_spec.freq, (short **)&m_data)) > 0)
    {
        m_file.setType(File::T_OGG);
        m_spec.channels = channels_int;
        m_spec.format = AUDIO_S16;
    }

    SDL_ClearError();
    SDL_FreeRW(audio_rw);
}

AudioData::AudioData(const char *path): Loaded(this)
{
    m_file.Load(path);

    if (!m_file.getSize())
    { /*Will do something*/
    }
}

AudioData::~AudioData()
{
    thrd.Detach();
    if (m_data)
        free((void *)m_data);
}