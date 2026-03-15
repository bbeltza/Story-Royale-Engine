#include <Base/AudioChunk.h>
#include <Base/File.h>
#include <Base/Chunk.h>

#include <utils/mem.h>

#include <SDL_audio.h>
#include <vorbis.h>
#include <assert.h>

const size_t SRE_AUDIOCHUNK_METASIZE = offsetof(sre_AudioChunk, samples);

const sre_AudioChunk* sre_audioload(size_t size, const sre_byte* rawdata)
{
    // Only check RIFF header for .wav, we know that it won't be any other audio format
    // so if the file is not an audio file then SDL_LoadWAV_RW fails
    if (
        rawdata[0] == 'R' &&
        rawdata[1] == 'I' &&
        rawdata[2] == 'F' &&
        rawdata[3] == 'F'
    )
    {
        SDL_RWops* rw = SDL_RWFromConstMem(rawdata, (int)size);
        SDL_AudioSpec spec;
        Uint8* audio_buff;
        Uint32 audio_len;

        if (!SDL_LoadWAV_RW(rw, 1, &spec, &audio_buff, &audio_len)) return NULL;

        const sre_AudioChunk* chunk = sre_audiofromraw(&(sre_AudioChunk){
            .format = spec.format,
            .channels = spec.channels,
            .frequency = spec.freq,
            .size = audio_len,
            .sample_count = audio_len / (SDL_AUDIO_BITSIZE(spec.format)/8) / spec.channels
        }, audio_buff);

        SDL_FreeWAV(audio_buff);
        return chunk;
    }
    else if (
        rawdata[0] == 'O' &&
        rawdata[1] == 'g' &&
        rawdata[2] == 'g' &&
        rawdata[3] == 'S'
    )
    {
        int audio_samples;
        int audio_len;
        int channels;
        int sample_rate;
        short* output;

        audio_samples = stb_vorbis_decode_memory(rawdata, (int)size, &channels, &sample_rate, &output);
        if (audio_samples < 0) return NULL;

        audio_len = audio_samples * channels * (SDL_AUDIO_BITSIZE(AUDIO_S16) / 8);

        const sre_AudioChunk* chunk = sre_audiofromraw(&(sre_AudioChunk){
            .format = AUDIO_S16,
            .frequency = sample_rate,
            .sample_count = audio_samples,
            .size = audio_len,
            .channels = channels
        }, output);

        free(output);

        return chunk;
    }

    return NULL;
}

int sre_audioaddref(const sre_AudioChunk* chunk) { return SDL_AtomicAdd((SDL_atomic_t*)&chunk->_refcount, 1); }

int sre_audioclose(const sre_AudioChunk* chunk)
{
    if (!chunk)
        return -1;

    int ref = SDL_AtomicAdd((SDL_atomic_t*)&chunk->_refcount, -1);
    if (ref <= 1)
        sre_delete((void*)chunk);
    
    assert(ref >= 0);
    return ref;
}

const sre_AudioChunk* sre_audiofromfile(const sre_File* file)
{
    if (!file) return NULL;
    const sre_byte* file_data = sre_filebegin(file);
    if (file_data)
        return sre_audioload(sre_filesize(file), file_data);
    
    const sre_Chunk *fchunk = sre_fileallocate(file, 0);
    if (!fchunk) return NULL;

    const sre_AudioChunk *achunk = sre_audioload(fchunk->size, fchunk->data);

    sre_chunkfree(fchunk);
    return achunk;
}

const sre_AudioChunk* sre_audiofromraw(const sre_AudioChunk* metadata, const void* raw)
{
    if (!metadata)
        return NULL;
    
    const size_t size = metadata->size;
    sre_AudioChunk* chunk = sre_new(SRE_AUDIOCHUNK_METASIZE + size);
    chunk->_refcount = 0;
    chunk->size = size;
    chunk->sample_count = metadata->sample_count;
    chunk->format = metadata->format;
    chunk->channels = metadata->channels;
    chunk->frequency = metadata->frequency;
    memcpy(chunk->samples, raw, size);

    return chunk;
}