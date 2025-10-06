#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <SDL.h>

#define defamt(m) double amt = x / (float)m

#define defamt64 defamt(0x7FFFFFFFFFFFFFFF)
#define defamt32 defamt(0x7FFFFFFF)
#define defamt16 defamt(0x7FFF)
#define defamt8 defamt(0x7F)

#define reti8 return (int8_t)(amt * 0x7F);
#define reti16 return (int16_t)(amt * 0x7FFF);
#define reti32 return (int32_t)(amt * 0x7FFFFFFF);
#define reti64 return (int64_t)(amt * 0x7FFFFFFFFFFFFFFF);

static inline int32_t format_i64_i32(int64_t x) { defamt64; reti32 }
static inline int32_t format_i16_i32(int16_t x) { defamt16; reti32 }
static inline int32_t format_i8_i32(int8_t x) { defamt8; reti32 }

static inline int16_t format_i64_i16(int64_t x) { defamt64; reti16 }
static inline int16_t format_i32_i16(int32_t x) { defamt32; reti16 }
static inline int16_t format_i8_i16(int8_t x) { defamt8; reti16 }

int ConvertAudioFormat(SDL_AudioFormat f_input, SDL_AudioFormat f_output, int8_t** data, int len_input)
{
    if (f_input == f_output) return len_input;

    uint8_t out_bitsize = SDL_AUDIO_BITSIZE(f_output), in_bitsize = SDL_AUDIO_BITSIZE(f_input);

    float fac = out_bitsize / (float)in_bitsize;
    int len_output = (int)(len_input * fac);
    
    int8_t* out_data = malloc(len_output);

    switch (in_bitsize)
    {
        case 8:
            for (int i = 0; i < len_input; i++)
            {
                if (out_bitsize == 16)  ((int16_t*)out_data)[i] = format_i8_i16((*data)[i]);
                else if (out_bitsize == 32) ((int32_t*)out_data)[i] = format_i8_i32((*data)[i]);
            }
            break;
        case 16:
            for (int i = 0; i < len_input / 2; i++)
            {
                if (out_bitsize == 32) ((int32_t*)out_data)[i] = format_i16_i32(((int16_t*)*data)[i]);
            }
    }

    free(*data);
    *data = out_data;

    return len_output;
}