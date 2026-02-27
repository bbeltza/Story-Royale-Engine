#ifndef SRE_RWOPS_H
#define SRE_RWOPS_H // SDL RWops utilities
#include <C_API.h>
#include <SDL_rwops.h>

SRE_CAPI_BEGIN

struct sre_File;

SDL_RWops* sre_filetorwops(const struct sre_File* file);

SRE_CAPI_END

#endif