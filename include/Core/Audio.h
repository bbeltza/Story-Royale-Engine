#ifndef SRE_AUDIO_H
#define SRE_AUDIO_H
#include <C_API.h>
#include <ints.h>

SRE_CAPI_BEGIN

typedef struct sre_AudioChunk sre_AudioChunk;
// Return a copy of an AudioChunk that respects the format of the audio engine
//
// Note that the `chunk`'s frequency will not be changed after converting it, it still respects the format of the audio engine.
// So you'll have to use `sre_audiofreq()` to retrieve the engine's frequency, or the helper `sre_audiofreqratio()` function.
//
// @param chunk The chunk to convert
// @param freechunk `true` if you want `chunk` to be freed
// @return The new AudioChunk, if the passed `chunk` doesn't respect the audio engine's format.
// @return `chunk` if it respects the audio engine's format, so it won't be freed if `freechunk` is on.
// @return `NULL` if the function fails.
const sre_AudioChunk* sre_convertchunk(const sre_AudioChunk* chunk, bool freechunk);

int sre_audiocallbackqueue(int (*callback)(void* userdata, sre_u8* samples, sre_usize size) , void* userdata);
void sre_audiocallbackremove(int id);

// Return the sample-rate of the audio engine
int sre_audiofreq();

// Return `freq` / `sre_audiofreq()`
double sre_audiofreqratio(int freq);

void sre_audiosetmaster(unsigned vol);
int sre_audiogetmaster();

// Closes the current audio device, and creates a new one with these parameters
bool sre_audioconfigure(bool mono, int freq);

SRE_CAPI_END

#endif