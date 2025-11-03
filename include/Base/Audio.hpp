#pragma once
#include "Base/File.hpp"
#include "Base/Signal.hpp"
#include "Base/Thread.hpp"

#include "Datatypes/TimeStamp.h"

#include "internal_def.hh"

#include <SDL.h>

#define AUDIO_BYTESIZE(x) (SDL_AUDIO_BITSIZE(x)/8)

__def_internal(__update_audio)

class AudioData
{
	friend class Audio;

	AudioData() = delete;
	AudioData(const char* path);

	void Load();
	void Unload();

	File m_file;
	
	Thread thrd;

	int8_t* m_data = nullptr;
	uint32_t m_len;
	SDL_AudioSpec m_spec;

	SDL_RWops* m_rw;

	bool m_loaded = false;

public:
	~AudioData();

	Signal<> Loaded;

	inline uint32_t len() const { return m_len; }
	inline uint32_t freq() const { return m_spec.freq; }

	__friend_internal(__update_audio)
};

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
public:
	AudioInfo Info;
public:
	static AudioData& Load(const char* path);
	AudioData& Attach(AudioData& data)
	{
		m_data = &data;
		return data;
	}

	bool IsPlaying() const;

	TimeStamp timePosition() const { return static_cast<TimeStamp>(m_fsamplepos / m_data->freq()); }
	TimeStamp timeLength() const { return static_cast<TimeStamp>(m_data->len() / (TimeStamp)m_data->freq()); }

	void Play(bool force=false);
	void FadeOut() { m_fadeout = true; }

	TimeStamp Pause();
	TimeStamp Stop();

private:
	static void threadedload(AudioData* audio);
	AudioData* m_data = nullptr;

	double m_fsamplepos = 0;  // The position of the sample with subsample precition
	uint32_t m_samplepos = 0; // The position (in samples) of the audio sample

	bool m_fadein = false, m_fadeout = false;
	float m_fadevol = 1;

	__friend_internal(__update_audio)
};