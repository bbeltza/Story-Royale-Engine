#ifndef SRE_AUDIOCHUNK_H
#define SRE_AUDIOCHUNK_H
#include <C_API.h>

#include <ints.h>

SRE_CAPI_BEGIN

typedef unsigned short sre_sample;

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
extern const sre_usize SRE_AUDIOCHUNK_METASIZE;

const sre_AudioChunk* sre_audioload(size_t size, const sre_byte* rawdata);
void sre_audioclose(const sre_AudioChunk* chunk);

typedef struct sre_File sre_File;
const sre_AudioChunk* sre_audiofromfile(const sre_File* file);

SRE_CAPI_END

#endif