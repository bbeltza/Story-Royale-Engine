#ifndef SRE_RWOPS_H
#define SRE_RWOPS_H // SDL RWops utilities
#include <C/API.h>
#include <SDL_rwops.h>

_CAPI_BEGIN

typedef struct sre_File sre_File;

SDL_RWops* sre_filetorwops(const sre_File* file);

_CAPI_END

#endif