#include <Base/Audio.hpp>
#include <Base/Audio.h>
#include <Base/File.hpp>
#include <Base/Thread.hpp>

#include <utils/logging.h>

using namespace sre;

const audiostyle Audio::DEFAULT_STYLE;

void Audio::play()
{
	if (!m_chunk)
	{
		WARN("Calling audio::play() while no chunk has been attached");
		return;
	}
	if (!m_id)
		m_id = sre_audiocallbackqueue(audio_callback, this);
	
	m_playing = 1;
	m_fading = 2;
}

sre::timeStamp Audio::stop()
{
	sre::timeStamp ts = pause();
	m_samplepos = 0;
	m_fsamplepos = 0;
	
	return ts;
}

sre::timeStamp Audio::pause()
{
	m_playing = 0;
	return time_pos();
}

const sre_AudioChunk* Audio::load(const char* from_path)
{
	File file(from_path);
	return load(file);
}

const sre_AudioChunk* Audio::load(const File& from_file)
{
	const sre_AudioChunk* chunk = from_file.call_cfunc(sre_audiofromfile);
	if (!chunk)
	{
		WARN("Audio::load() failed loading the audio file");
		return NULL;
	}

	chunk = sre_convertchunk(chunk, true);
	if (!chunk)
	{
		WARN("Audio::load() failed converting the audio file to the engine's format");
		return NULL;
	}

	m_chunk = chunk;
	return chunk;
}

int Audio::audio_callback(void* userdata, sre_u8* _samples, sre_usize len)
{
	Audio* _this = static_cast<Audio*>(userdata);
	if (!_this->m_playing) return 0;

	sre_s16* samples = reinterpret_cast<sre_s16*>(_samples);
	len >>= 1;

	for (sre_usize i = 0; i < len; i++)
	{
		samples[i] = _this->m_chunk->samples[_this->m_samplepos];
		
		_this->m_samplepos++;
		if (_this->m_samplepos >= _this->m_chunk->sample_count)
		{
			_this->m_playing = 0;
			_this->m_samplepos = 0;
			break;
		}
	}

	return 128;
}