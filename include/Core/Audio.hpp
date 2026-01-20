#ifndef SRE_AUDIO_HPP
#define SRE_AUDIO_HPP
#include <Core/Audio.h>
#include <Base/AudioChunk.h>
#include <Datatypes/TimeStamp.h>

namespace sre
{
	inline AudioChunk convertchunk(const AudioChunk& chunk)
	{
		AudioChunk converted = chunk;
		converted = sre_convertchunk(chunk.get(), false);
		return converted;
	}

	template <typename Cls>
	inline int audio_callbackqueue(int (*callback)(Cls* userdata, byte* samples, usize size), Cls* userdata)
	{
		return sre_audiocallbackqueue(reinterpret_cast<int(*)(void*, byte*, usize)>(callback), userdata);
	}
	inline int audio_callbackqueue(int (*callback)(void* userdata, byte* samples, usize size), void* userdata) { return sre_audiocallbackqueue(callback, userdata); }
	inline void audio_callbackremove(int id) { return sre_audiocallbackremove(id); }

	inline int audio_frequency() { return sre_audiofreq(); }
	inline double audio_freqratio(int freq) { return sre_audiofreqratio(freq); }

	inline void audio_setmaster(unsigned vol) { return sre_audiosetmaster(vol); }
	inline void audio_setmaster(float vol) { return sre_audiosetmaster(static_cast<unsigned>(128 * vol)); }

	inline int audio_getmaster() { return sre_audiogetmaster(); }
	template <typename T>
	inline T audio_getmaster() { static_assert(0, "Invalid use of audio_getmaster<T>(), use audio_getmaster<float>() instead"); }
	template <>
	inline float audio_getmaster<float>() { return sre_audiogetmaster() / 128.0f; }
}

namespace sre
{
	// Constant to indicate that an audio should not loop.
	// Any value higher than the sample length of the audio indicates that the audio won't loop.
	// The `-1` unsigned int bit representation is `0xFFFFFFFF`, the highest unsigned number so it can be used to indicate that.
	constexpr unsigned AUDIO_DONTLOOP = -1;

	class Audio
	{
		AudioChunk m_chunk;
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
		static int audio_callback(Audio* audio, sre_u8* samples, sre_usize len);
	public:
		constexpr Audio() = default;
		Audio(const AudioChunk& chunk): m_chunk(chunk) {}

		Audio(const Audio& copy): m_chunk(copy.m_chunk) {}

		void attach_chunk(const AudioChunk& chunk) { m_chunk = chunk; }

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

		AudioChunk load(const char* from_path);
		AudioChunk load(const File& from_file);
	};
}

#endif