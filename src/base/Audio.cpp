#include <SDL.h>
#include <Base/Audio.hpp>
#include <Base/Thread.hpp>

#include <vorbis.h>
#include <utils/math.h>

#include "../internal.h"
#include "../internal.hpp"

void AudioData::Load()
{
	sre::File _f(path);
	sre::Chunk _data_chunk;
	const void* data = _f.resource_data();
	int size;
	if (!data)
	{
		_data_chunk = _f.allocate();
		data = _data_chunk->data;
		size = static_cast<int>(_data_chunk->size);
	}
	else
		size = static_cast<int>(_f.size());

    SDL_RWops *audio_rw = SDL_RWFromConstMem(data, size);
    
    int channels_int;
    Uint8 *stream_data;
    SDL_zero(m_spec);
    
    if (SDL_LoadWAV_RW(audio_rw, 0, &m_spec, &stream_data, &m_len))
    {
        m_data = (int8_t *)malloc(m_len);
        memcpy(m_data, stream_data, m_len);
        SDL_FreeWAV(stream_data);
        m_len /= AUDIO_BYTESIZE(m_spec.format);
        m_len /= m_spec.channels;
    }
    else if ((m_len = stb_vorbis_decode_memory((uint8_t *)data, size, &channels_int, &m_spec.freq, (short **)&m_data)) > 0)
    {
        m_spec.channels = channels_int;
        m_spec.format = AUDIO_S16;
    }

    SDL_ClearError();
    SDL_FreeRW(audio_rw);
}

void AudioData::Unload()
{
	_containers->loaded_audios.remove_if([&](const AudioAccess& _Other) -> bool { return &_Other.data == this; });;
	abort();
}

AudioData::AudioData(const char *path): path(path)
{
}

AudioData::~AudioData()
{
    thrd.detach();
    if (m_data)
        free((void *)m_data);
}

AudioData& Audio::Load(const char *path)
{
	_containers->loaded_audios.emplace_front(path);
	AudioData& audio = _containers->loaded_audios.front().data;
	audio.thrd = sre::Thread(threadedload, &audio);

	return audio;
}

sre::sptr Audio::threadedload(AudioData *audio)
{
	audio->Load();

	SDL_AudioCVT cvt;
	SDL_BuildAudioCVT(&cvt, audio->m_spec.format, audio->m_spec.channels, audio->m_spec.freq,
							engine.audio_spec.format, audio->m_spec.channels, audio->m_spec.freq);
	if (!cvt.needed) goto FINISH;

	cvt.len = audio->m_len * audio->m_spec.channels * AUDIO_BYTESIZE(audio->m_spec.format);
	cvt.buf = cvt.len_mult > 1 ? static_cast<Uint8*>(realloc(audio->m_data, cvt.len * cvt.len_mult)) : reinterpret_cast<Uint8*>(audio->m_data);
	SDL_ConvertAudio(&cvt);
	
	audio->m_data = cvt.len_ratio < 1.0 ? static_cast<int8_t*>(realloc(cvt.buf, cvt.len_cvt)) : reinterpret_cast<int8_t*>(cvt.buf);

	FINISH:
	audio->m_spec.format = engine.audio_spec.format;
	audio->m_loaded = true;

	audio->Loaded.Fire();

	return 0;
}

void Audio::Play(bool force)
{
	if (!engine.containers_service) return;
	if (!m_data)
	{
		ERROR("Audio::Play(): Could not play audio, no data has been attached");
		return;
	}

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

	_containers->audio_queue.insert(this);

	if (Info.fade_in > 0)
	{
		m_fadein = true;
		m_fadevol = 0;
	}
	m_fadeout = false;

	SDL_PauseAudioDevice(engine.audio_device, 0);

	SDL_UnlockAudioDevice(engine.audio_device);
}

sre::timeStamp Audio::Pause()
{
	SDL_LockAudioDevice(engine.audio_device);

	_containers->audio_queue.erase(this);
	if (_containers->audio_queue.empty())
		SDL_PauseAudioDevice(engine.audio_device, 1);

	SDL_UnlockAudioDevice(engine.audio_device);

	return timePosition();
}

sre::timeStamp Audio::Stop()
{
	auto ts = Pause();
	m_fsamplepos = 0;
	m_samplepos = 0;
	return ts;
}

bool Audio::IsPlaying() const
{
	return engine.containers_service && _containers->audio_queue.count(const_cast<Audio *>(this)) != 0;
}