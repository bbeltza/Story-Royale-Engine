#pragma once
#include <SDL_audio.h>
#include <stdint.h>
#include <C/API.h>

_CAPI_BEGIN
extern uint8_t* ut_loadwavaudio(const uint8_t* data, int size, size_t* out_size);
extern size_t ut_audioconvertformat(SDL_AudioFormat f_input, SDL_AudioFormat f_output, int8_t** data, size_t len_input);
_CAPI_END