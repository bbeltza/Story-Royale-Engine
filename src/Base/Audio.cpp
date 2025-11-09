#include <SDL.hpp>
#include "Engine.hpp"
#include "Base/Audio.hpp"
#include "Base/Thread.hpp"

#include "vorbis.h"
#include "utils.h"
#include "../internal.h"

//

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
        m_data = (int8_t *)malloc(m_len);
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

void AudioData::Unload()
{
	for (auto &kv : *_audio_loaded)
	{
		if (kv.second.get() == this)
			return (void)_audio_loaded->erase(kv.first);
	}
	abort();
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

AudioData& Audio::Load(const char *path)
{
	AudioData *audio;

	if (_audio_loaded->count(path) == 0)
	{
		_audio_loaded->emplace(path, new AudioData(path));
		audio = _audio_loaded->at(path).get();
		audio->thrd = Threads::Create(threadedload, audio);
	}
	else
		audio = _audio_loaded->at(path).get();

	return *audio;
}

//

extern "C" uintptr_t ConvertAudioFormat(SDL_AudioFormat f_input, SDL_AudioFormat f_output, int8_t **d_input, intptr_t len_input);
void Audio::threadedload(AudioData *audio)
{
	audio->Load();
	ConvertAudioFormat(audio->m_spec.format, engine.audio_spec.format, &audio->m_data, audio->m_len * audio->m_spec.channels * AUDIO_BYTESIZE(audio->m_spec.format));

	audio->m_spec.format = engine.audio_spec.format;
	audio->m_loaded = true;

	audio->Loaded.Fire();
}

void Audio::Play(bool force)
{
	if (!m_data->m_loaded)
		m_data->Loaded.Wait();
	SDL_LockAudioDevice(engine.audio_device);
	if (force)
		Stop();
	else if (IsPlaying())
	{
		SDL_UnlockAudioDevice(engine.audio_device);
		return;
	}

	_audio_queue->insert(this);

	if (Info.fade_in > 0)
	{
		m_fadein = true;
		m_fadevol = 0;
	}
	m_fadeout = false;

	SDL_PauseAudioDevice(engine.audio_device, 0);

	SDL_UnlockAudioDevice(engine.audio_device);
}

TimeStamp Audio::Pause()
{
	SDL_LockAudioDevice(engine.audio_device);

	_audio_queue->erase(this);
	if (_audio_queue->empty())
		SDL_PauseAudioDevice(engine.audio_device, 1);

	SDL_UnlockAudioDevice(engine.audio_device);

	return timePosition();
}

TimeStamp Audio::Stop()
{
	auto ts = Pause();
	m_fsamplepos = 0;
	m_samplepos = 0;
	return ts;
}

bool Audio::IsPlaying() const
{
	return _audio_queue->count(const_cast<Audio *>(this)) != 0;
}