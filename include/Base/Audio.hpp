#pragma once
#include <Base/AudioChunk.h>
#include <Datatypes/TimeStamp.h>

namespace sre
{
	// Audio.h wrappers here :3
}

namespace sre
{
	// Constant to indicate that an audio should not loop.
	// Any value higher than the sample length of the audio indicates that the audio won't loop.
	// The `-1` unsigned int bit representation is `0xFFFFFFFF`, the highest unsigned number so it can be used to indicate that.
	constexpr unsigned AUDIO_DONTLOOP = -1;

	class File;

	class Audio
	{
		const sre_AudioChunk* m_chunk = NULL;
	public:
		enum playingState
		{
			STATE_STOPPED,
			STATE_PLAYING
		};
	public:
		unsigned loop_point = AUDIO_DONTLOOP;
		
		float volume = 1;
		timeStamp speed = 1;
	private:
		u32 m_samplepos = 0;
		timeStamp m_subsample = 0;
		int m_id = 0;

		float m_fadevol = 1;
		float m_fading = 0; // Fading state

		int m_state = 0; // Playing state
	private:
		static int audio_callback(void* userdata, sre_u8* samples, sre_usize len);
	public:
		constexpr Audio() = default;
		Audio(const sre_AudioChunk* chunk): m_chunk(chunk) {}

		constexpr Audio(const Audio& copy): m_chunk(copy.m_chunk) {}

		void attach_chunk(const sre_AudioChunk* chunk) { m_chunk = chunk; }

		void play();
		void replay();
		void pause();
		sre::timeStamp stop();

		void fade_in(float fade)
		{
			// Guess let's just try this...
			auto bits = ~(1 << (sizeof(fade) - 1));
			bits &= reinterpret_cast<int&>(fade);
			m_fading = reinterpret_cast<float&>(bits);
			m_fadevol = 0;
			play();
		}
		void fade_out(float fade)
		{
			int bits = reinterpret_cast<int&>(fade) | 0x80000000;
			m_fading = reinterpret_cast<float&>(bits);
		}

		sre::timeStamp time_pos() const
		{
			if (!m_chunk) return 0;
			return static_cast<sre::timeStamp>((m_samplepos + m_subsample) / m_chunk->frequency);
		}
		sre::timeStamp time_len() const
		{
			if (!m_chunk) return 0;
			return static_cast<sre::timeStamp>(m_chunk->sample_count) / m_chunk->frequency;
		}

		const sre_AudioChunk* load(const char* from_path);
		const sre_AudioChunk* load(const File& from_file);
	};
}