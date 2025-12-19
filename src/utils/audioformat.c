#include <standard>

#include "utils/audioutils.h"
#include "utils/logging.h"

#include "SDL_timer.h"

#define reti8 return (int8_t)(amt * 0x7F);
#define reti16 return (int16_t)(amt * 0x7FFF);
#define reti32 return (int32_t)(amt * 0x7FFFFFFF);
#define reti64 return (int64_t)(amt * 0x7FFFFFFFFFFFFFFF);

#define format_int(x, m, y) ((x / (double)m) * y)

#define format_i8(x, m) format_int(x, m, 0x7F)
#define format_i16(x, m) format_int(x, m, 0x7FFF)
#define format_i32(x, m) format_int(x, m, 0x7FFFFFFF)
#define format_i64(x, m) format_int(x, m, 0x7FFFFFFFFFFFFFFF)

size_t ut_audioconvertformat(SDL_AudioFormat f_input, SDL_AudioFormat f_output, int8_t** data, size_t len_input)
{
    if (f_input == f_output) return len_input;

    uint32_t start = SDL_GetTicks();

    int out_bitsize = SDL_AUDIO_BITSIZE(f_output), in_bitsize = SDL_AUDIO_BITSIZE(f_input);

    float fac = out_bitsize / (float)in_bitsize;
    intptr_t len_output = (intptr_t)(len_input * fac);

    int8_t* out_data = malloc(len_output);
    if (!out_data)
    {
        ERROR("ut_audioconvertformat(): Failed to allocate enough memory, heap is probably out of space, returning original block");
        return len_input;
    }

    switch (in_bitsize)
    {
        case 8:
            for (uintptr_t i = 0; i < len_input; i++)
            {
                if (out_bitsize == 16)  ((int16_t*)out_data)[i] = (int16_t)format_i16((*data)[i], 0x7F);
                else if (out_bitsize == 32) ((int32_t*)out_data)[i] = (int32_t)format_i32((*data)[i], 0x7F);
            }
            break;
        case 16:
            len_input /= 2;
            for (uintptr_t i = 0; i < len_input; i++)
            {
                if (out_bitsize == 32)
                    ((int32_t*)out_data)[i] = (int32_t)format_i32(((int16_t*)*data)[i], 0x7FFF);
            }
        case 32:
            len_input /= 4;
            for (uintptr_t i = 0; i < len_input; i++)
            {
                if (out_bitsize == 16)
                    ((int16_t*)out_data)[i] = (int16_t)format_i16(((int32_t*)*data)[i], 0x7FFFFFFF);
            }
    }

    free(*data);
    *data = out_data;

    uint32_t end = SDL_GetTicks();
    //printf("Audio converted, lasted %f seconds\n", (float)(end - start)/1000);

    (void)start;
    (void)end;

    return len_output;
}