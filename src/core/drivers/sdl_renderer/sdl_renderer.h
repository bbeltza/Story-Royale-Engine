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

extern sre_sdlrenderer sresdlrenderer_driver;

extern sre_videodriver* sresdlrenderer_init(SDL_Window*);
extern int sresdlrenderer_vsync(int vsync);
extern void sresdlrenderer_present();

extern int sresdlrenderer_draw_fill(const sre_DDFill* data);
extern int sresdlrenderer_draw_line(const sre_DDLine* data);
extern int sresdlrenderer_draw_lines(const sre_DDLines* data);

SRE_CAPI_END

#endif