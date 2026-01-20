#include <Base/Audio.hpp>
#include <Base/Audio.h>
#include <Base/File.hpp>
#include <Base/Thread.hpp>

#include <utils/logging.h>
#include <utils/math.hpp>

using namespace sre;

AudioChunk::AudioChunk(const File& from_file): AudioChunk(from_file.call_cfunc(sre_audiofromfile))
{
}

void Audio::play()
{
	if (!m_chunk)
	{
		WARN("Calling audio::play() while no chunk has been attached");
		return;
	}
	if (!m_id)
		m_id = audio_callbackqueue(audio_callback, this);

	m_state = STATE_PLAYING;
}

void Audio::replay()
{
	m_samplepos = 0;
	m_subsample = 0;
	play();
}

sre::timeStamp Audio::stop()
{
	pause();
	sre::timeStamp ts = time_pos();
	m_samplepos = 0;
	m_subsample = 0;

	return ts;
}

void Audio::pause()
{
	m_state = STATE_STOPPED;
}

AudioChunk Audio::load(const char* from_path)
{
	File file(from_path);
	return load(file);
}

AudioChunk Audio::load(const File& from_file)
{
	AudioChunk chunk{from_file};
	if (!chunk)
	{
		WARN("Audio::load() failed loading the audio file");
		return NULL;
	}

	chunk = sre::convertchunk(chunk);
	if (!chunk)
	{
		WARN("Audio::load() failed converting the audio file to the engine's format");
		return NULL;
	}

	m_chunk = chunk;
	return chunk;
}

int Audio::audio_callback(Audio* audio, sre_u8* _samples, sre_usize len)
{
	if (!audio->m_state) return 0;

	s16* samples = reinterpret_cast<s16*>(_samples);
	len >>= 1;

	const int channels = audio->m_chunk->channels;
	const int freq = audio_frequency();
	const timeStamp freqratio = static_cast<timeStamp>(audio_freqratio(audio->m_chunk->frequency));
	
	for (sre_usize i = 0; i < len; i += channels)
	{
		u32 sampleindex = audio->m_samplepos * channels;
		const s16* const chunksamples = audio->m_chunk->samples + sampleindex;

		if (audio->m_fading)
		{
			audio->m_fadevol += 1 / (audio->m_fading * freq);
			if (audio->m_fadevol >= 1)
			{
				audio->m_fadevol = 1;
				audio->m_fading = 0;
			}
			else if (audio->m_fadevol <= 0)
			{
				audio->m_fadevol = 1;
				audio->m_fading = 0;
				goto STOP_SECTION;
			}
		}

		for (int c = 0; c < channels; c++)
			samples[i + c] = static_cast<s16>(chunksamples[c] * audio->m_fadevol);

		if (!audio->m_subsample) goto DONT_INTERPOLATE;
		for (int c = 0; c < channels; c++)
		{
			s16 nextsample = static_cast<s16>(chunksamples[c + channels] * audio->m_fadevol);
			samples[i + c] = ut::lerp(samples[i + c], nextsample, audio->m_subsample);
		}

		DONT_INTERPOLATE:

		audio->m_subsample += freqratio * audio->speed;
		{
			u32 truncated_subsample = static_cast<u32>(audio->m_subsample);
			audio->m_samplepos += truncated_subsample;
			audio->m_subsample -= truncated_subsample;
		}

		if (audio->m_samplepos >= audio->m_chunk->sample_count)
		{
			if (audio->loop_point > audio->m_chunk->sample_count)
				goto STOP_SECTION;
			
			audio->m_samplepos = audio->loop_point;
		}
		continue;

		STOP_SECTION:
		audio->m_state = STATE_STOPPED;
		audio->m_samplepos = 0;
		audio->m_subsample = 0;
		break;
	}

	return static_cast<int>(audio->volume * 128);
}