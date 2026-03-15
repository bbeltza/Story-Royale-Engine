#ifndef SRE_AUDIOCHUNK_H
#define SRE_AUDIOCHUNK_H
#include <C_API.h>

#include <ints.h>

SRE_CAPI_BEGIN

typedef signed short sre_sample;

typedef struct sre_AudioChunk
{
	int _refcount;

    sre_u32 size;
    sre_u32 sample_count;

    sre_u16 format;
    sre_u16 channels;
    sre_u32 frequency;

    sre_sample samples[
		#ifdef __cplusplus
			1
		#endif
	];
} sre_AudioChunk;
// Size of the audio chunk metadata (the entire size without `samples`)
extern const size_t SRE_AUDIOCHUNK_METASIZE;

const sre_AudioChunk* sre_audioload(size_t size, const sre_byte* rawdata);
int sre_audioaddref(const sre_AudioChunk* chunk);
int sre_audioclose(const sre_AudioChunk* chunk);

const sre_AudioChunk* sre_audiofromraw(const sre_AudioChunk* metadata, const void* data);

struct sre_File;
const sre_AudioChunk* sre_audiofromfile(const struct sre_File* file);
const sre_AudioChunk* sre_audiofromcache(const char* resource);
bool sre_audiotocache(const sre_AudioChunk* chunk, const char* resource);

SRE_CAPI_END

#ifdef __cplusplus
#include <memory>

namespace sre
{
    class File;
    // Holder class for the audio chunk (sre_AudioChunk shared pointer wrapper)
	class AudioChunk
	{
		const sre_AudioChunk* m_ptr = NULL;
	public:
		constexpr AudioChunk() = default;
		AudioChunk(const sre_AudioChunk* chunk): m_ptr(chunk) { sre_audioaddref(chunk); }
		AudioChunk(const AudioChunk& chunk): AudioChunk(chunk.m_ptr) {}

		void operator =(const AudioChunk& chunk)
		{
			sre_audioclose(m_ptr);
			sre_audioaddref(chunk.m_ptr);
			m_ptr = chunk.m_ptr;
		}

		~AudioChunk() { sre_audioclose(m_ptr); m_ptr = NULL; }

		//void operator =(const AudioChunk& chunk) { m_ptr = chunk.m_ptr; sre_audioaddref(chunk); }

		const sre_AudioChunk* operator ->() const { return m_ptr; }
		operator bool() const { return m_ptr != NULL; }

        const sre_AudioChunk* get() const { return m_ptr; }
	public:
		AudioChunk(size_t size, const byte* rawdata): AudioChunk(sre_audioload(size, rawdata)) {}
		AudioChunk(const File& file);
	};
}

#endif

#endif