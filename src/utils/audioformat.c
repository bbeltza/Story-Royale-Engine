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

#define INT_T(bs) int##bs##_t

#define CASE_DEFAULT() default: abort()
#define CASE_M(bs, obs, M) case bs: for(size_t i = 0; i < len_input; i++) ((INT_T(bs)*)out_data)[i] = (INT_T(bs))format_i##bs(((INT_T(obs)*)*data)[i], M); break

#define CASE_I8(bs) CASE_M(bs, 8, 0x7F)
#define CASE_I16(bs) CASE_M(bs, 16, 0x7FFF)
#define CASE_I32(bs) CASE_M(bs, 32, 0x7FFFFFFF)
#define CASE_I64(bs) CASE_M(bs, 64, 0x7FFFFFFFFFFFFFFF)

#define IN_CASE(bs, shift, ...) case bs: len_input >>= shift; switch (out_bitsize) { __VA_ARGS__ CASE_DEFAULT(); } break;

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
        IN_CASE(8, 0, 
            CASE_I8(16);
            CASE_I8(32);
            CASE_I8(64);
        )
        IN_CASE(16, 1, 
            CASE_I16(8);
            CASE_I16(32);
            CASE_I16(64);
        )
        IN_CASE(32, 2, 
            CASE_I32(8);
            CASE_I32(16);
            CASE_I32(64);
        )
        IN_CASE(64, 3, 
            CASE_I64(8);
            CASE_I64(16);
            CASE_I64(32);
        )
        default:
            abort();
            break;
    }

    free(*data);
    *data = out_data;

    uint32_t end = SDL_GetTicks();
    //printf("Audio converted, lasted %f seconds\n", (float)(end - start)/1000);

    (void)start;
    (void)end;

    return len_output;
}