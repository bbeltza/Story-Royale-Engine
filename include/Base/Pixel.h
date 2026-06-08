#ifndef SRE_PIXEL_H
#define SRE_PIXEL_H

#include <SDL_pixels.h>
typedef SDL_PixelFormatEnum sre_SDLpixelFormat;

#ifdef __cplusplus

namespace sre
{
    using SDLpixelFormat = sre_SDLpixelFormat;
}

#endif

#endif