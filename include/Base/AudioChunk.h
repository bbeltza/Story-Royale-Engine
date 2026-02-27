#ifndef SRE_AUDIOCHUNK_H
#define SRE_AUDIOCHUNK_H
#include <C_API.h>

#include <ints.h>

SRE_CAPI_BEGIN

typedef signed short sre_sample;

typedef struct sre_AudioChunk
{
    sre_u32 size;
    sre_u32 sample_count;

    sre_u16 format;
    sre_u16 channels;
    sre_u32 frequency;

    sre_sample samples[sizeof(sre_u32)];
} sre_AudioChunk;
// Size of the audio chunk metadata (the entire size without `samples`)
extern const size_t SRE_AUDIOCHUNK_METASIZE;

const sre_AudioChunk* sre_audioload(size_t size, const sre_byte* rawdata);
void sre_audioclose(const sre_AudioChunk* chunk);

struct sre_File;
const sre_AudioChunk* sre_audiofromfile(const struct sre_File* file);

SRE_CAPI_END

#ifdef __cplusplus
#include <memory>

namespace sre
{
    class File;
    // Holder class for the audio chunk (sre_AudioChunk shared pointer wrapper)
	class AudioChunk
	{
		std::shared_ptr<const sre_AudioChunk> m_ptr;
		struct ChunkDeleter
		{
			void operator ()(const sre_AudioChunk* chunk) { sre_audioclose(chunk); }
		};

	public:
		constexpr AudioChunk() = default;
		AudioChunk(const sre_AudioChunk* chunk): m_ptr(chunk, ChunkDeleter{}) {}

		void operator =(const sre_AudioChunk* chunk) { if (get() != chunk) m_ptr.reset(chunk, ChunkDeleter{}); }
		void operator =(const AudioChunk& chunk) { m_ptr = chunk.m_ptr; }

		const sre_AudioChunk* operator ->() const { return m_ptr.operator->(); }
		operator bool() const { return m_ptr.operator bool(); }

        const sre_AudioChunk* get() const { return m_ptr.get(); }
	public:
		AudioChunk(size_t size, const byte* rawdata): AudioChunk(sre_audioload(size, rawdata)) {}
		AudioChunk(const File& file);
	};
}

#endif

#endif