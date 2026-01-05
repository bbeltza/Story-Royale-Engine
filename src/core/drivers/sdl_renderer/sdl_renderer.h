#ifndef SRE_DRIVER_SDLRENDERER_H
#define SRE_DRIVER_SDLRENDERER_H
#include <drivers.h>
#include <C_API.h>

SRE_CAPI_BEGIN
typedef struct
{
    sre_videodriver video;
    
    SDL_Renderer* renderer;
    SDL_Texture* rect_tex;
} sre_sdlrenderer;

extern sre_sdlrenderer sre_SDLRendererDriver;

extern sre_videodriver* sresdlrenderer_init(SDL_Window*);
extern int sresdlrenderer_vsync(int vsync);
SRE_CAPI_END

#endif