#ifndef SRE_PIXEL_H
#define SRE_PIXEL_H

#include <SDL_pixels.h>
typedef SDL_PixelFormatEnum sre_pixelFormat;

#ifdef __cplusplus
namespace sre
{
    using pixelFormat = sre_pixelFormat;
}
#endif

#endif