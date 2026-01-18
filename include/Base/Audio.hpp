#pragma once
#include <Datatypes/AudioStyle.h>
#include <Base/AudioChunk.h>

namespace sre
{
	class File;

	class Audio
	{
		const sre_AudioChunk* m_chunk = NULL;
	public:
		const audiostyle* style;

		sre::timeStamp speed = 1;
	private:
		int m_id = 0;
		double m_fsamplepos = 0;  // The position of the sample with subsample precision
		uint32_t m_samplepos = 0; // The position (in samples) of the audio sample

		float m_fadevol = 1;
		short m_fading = 0; // Fading state
		short m_playing = 0; // Playing state
	public:
		static const audiostyle DEFAULT_STYLE;
	private:
		static int audio_callback(void* userdata, sre_u8* samples, sre_usize len);
	public:
		constexpr Audio(const audiostyle& style = DEFAULT_STYLE): style(&style) {}
		Audio(const sre_AudioChunk* chunk, const audiostyle& style = DEFAULT_STYLE): m_chunk(chunk), style(&style) {}

		constexpr Audio(const Audio& copy): style(copy.style), m_chunk(copy.m_chunk) {}

		void attach_chunk(const sre_AudioChunk* chunk) { m_chunk = chunk; }

		void play();
		sre::timeStamp pause();
		sre::timeStamp stop();

		void fade_out() { m_fading = 1; }

		sre::timeStamp time_pos() const
		{
			if (!m_chunk) return 0;

			return static_cast<sre::timeStamp>(m_fsamplepos / m_chunk->frequency);
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