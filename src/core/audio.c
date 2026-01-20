#include <Core/Audio.h>
#include <Base/AudioChunk.h>
#include <GameSettings.h>

#include <utils/mem.h>

#include "../internal.h"

int sre_audiofreq() { return engine.audio_spec.freq; }

double sre_audiofreqratio(int freq)
{
    return !engine.audio_spec.freq ? 0 : // Handle division by zero (As the audio engine may have not be initialized)
    (double)freq / (double)engine.audio_spec.freq;
}

void sre_audiosetmaster(unsigned volume) { engine.audio_master = volume < 128 ? volume : 128; }
int sre_audiogetmaster() { return engine.audio_master; }

struct sre_audiocallback
{
    int id;
    int (*callback)(void*, sre_u8*, sre_usize);
    void* userdata;
};

static void audio_callback(void *userdata, Uint8 *stream, int len)
{
    struct sre_audiocallback* queue = engine.audio_queue;
    assert(engine.audio_queue || !engine.audio_queuesize);

    if (!engine.audio_stream || engine.audio_slen < len)
    {
        engine.audio_stream = realloc(engine.audio_stream, len);
        engine.audio_slen = len;
    }
    assert(engine.audio_stream);
    memset(stream, 0, len);
    
    for (sre_usize i = 0; i < engine.audio_queuesize; i++)
    {
        memset(engine.audio_stream, 0, len);

        int vol = queue[i].callback(queue[i].userdata, engine.audio_stream, len);
        if (vol > 0)
            SDL_MixAudioFormat(stream, engine.audio_stream, engine.audio_spec.format, len, (engine.audio_master * vol) / 128);
    }
}

void __setup_audio_device()
{
	SDL_AudioSpec desiredspec = {0};
	desiredspec.callback = audio_callback;

    // Should make an enum for different frequencies instead of letting the user have free choice
	desiredspec.freq = game_settings.AudioOptions.Frequency;
	desiredspec.channels = 2 - game_settings.AudioOptions.Mono;
	desiredspec.samples = 512;
	desiredspec.format = AUDIO_S16;

	int CHANGES = 0;
	if (!desiredspec.freq)
		CHANGES |= SDL_AUDIO_ALLOW_FREQUENCY_CHANGE;

	engine.audio_device = SDL_OpenAudioDevice(NULL, 0, &desiredspec, &engine.audio_spec, CHANGES);
    engine.audio_master = SDL_MIX_MAXVOLUME;
}

int sre_audiocallbackqueue(int (*callback)(void* userdata, sre_u8* samples, sre_usize size) , void* userdata)
{
    if (!callback) return -1;

    SDL_LockAudioDevice(engine.audio_device);

    if (engine.audio_queuecap >= engine.audio_queuesize)
    {
        engine.audio_queuecap = engine.audio_queuecap ? engine.audio_queuecap * 2 : 32;
        engine.audio_queue = realloc(engine.audio_queue, sizeof(struct sre_audiocallback) * engine.audio_queuecap);
    }
    assert(engine.audio_queue);

    struct sre_audiocallback* obj = engine.audio_queue;
    obj = &obj[engine.audio_queuesize];
    obj->callback = callback;
    obj->userdata = userdata;
    obj->id = ++engine.last_audioid;

    engine.audio_queuesize++;

    SDL_PauseAudioDevice(engine.audio_device, 0);
    SDL_UnlockAudioDevice(engine.audio_device);

    return obj->id;
}

void sre_audiocallbackremove(int id)
{
    if (!id) return;

    SDL_LockAudioDevice(engine.audio_device);

    struct sre_audiocallback* queue = engine.audio_queue;
    for (sre_usize i = 0; i < engine.audio_queuesize; i++)
    {
        if (queue[i].id != id) continue;

        memmove(&queue[i], &queue[i + 1], engine.audio_queuesize - (i + 1));
        engine.audio_queuesize--;

        break;
    }

    SDL_PauseAudioDevice(engine.audio_device, !engine.audio_queuesize);
    SDL_UnlockAudioDevice(engine.audio_device);
}

const sre_AudioChunk* sre_convertchunk(const sre_AudioChunk* chunk, bool freechunk)
{
    SDL_AudioCVT cvt = {0};
    int needconv = SDL_BuildAudioCVT(&cvt, chunk->format, (Uint8)chunk->channels, chunk->frequency, engine.audio_spec.format, engine.audio_spec.channels, chunk->frequency);
    if (!needconv)
        return chunk;
    if (needconv < 0)
    {
        return NULL;
    }

    cvt.len = chunk->size;
    sre_AudioChunk* newchunk = sre_new(SRE_AUDIOCHUNK_METASIZE + cvt.len * cvt.len_mult);
    newchunk->frequency = chunk->frequency;
    newchunk->sample_count = chunk->sample_count;
    newchunk->channels = engine.audio_spec.channels;
    newchunk->format = engine.audio_spec.format;

    cvt.buf = (Uint8*)newchunk->samples;
    memcpy(cvt.buf, chunk->samples, chunk->size);

    SDL_ConvertAudio(&cvt);
    newchunk->size = cvt.len_cvt;

    if (freechunk)
        sre_audioclose(chunk);

    return newchunk;
}