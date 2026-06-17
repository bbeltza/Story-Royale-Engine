#ifndef SRE_SWRENDER_H
#define SRE_SWRENDER_H

#include <Core/Render.h>
#include <SDL_video.h>

typedef void (*sresw_blendfunc)(sre_col4* dst, const sre_col4* src);

typedef struct sresw_Texture
{
    SDL_Surface* staging;
    SDL_Surface* render;
} sresw_Texture;

typedef struct sresw_Instance
{
    SDL_Window* window;
    SDL_Surface* wndsurface;

    struct {
        int width;
        int height;

        int camerax;
        int cameray;

        int scale;

        int waitforrefresh;

        sresw_blendfunc curr_blendfunc;
    } state;
} sresw_Instance;

SRE_CAPI_BEGIN

extern void sresw_draw1(void* _inst, const sre_RenderInstance1* instances, size_t instance_count);
extern void sresw_draw2(void* _inst, const sre_RenderPoint* points, size_t point_count, sre_draw2primitive mode);

extern void sresw_begin(void* _inst, const float clear[4]);
extern void sresw_end(void* _inst);
    
extern void sresw_set_viewportstate(void* _inst, int w, int h, sre_unit scale);
extern void sresw_set_vsync(void* _inst, bool enable);
extern void sresw_set_texturestate(void* _inst, void* _texture);
extern void sresw_set_blendstate(void* _inst, sre_blendMode blendmode);
extern void sresw_set_camerastate(void* _inst, sre_unit x, sre_unit y);
extern void sresw_set_scissorstate(void* _inst, const sre_rect2Di* rectangle);

extern bool sresw_texture_setup(void* _inst, void* _texture, sre_SDLpixelFormat formathint, int w, int h, sre_SDLpixelFormat* outformat);
extern bool sresw_texture_update(void* _inst, void* _texture, const sre_rect2Di* region, const void* pixels, int pitch);
extern void sresw_texture_destroy(void* _inst, void* _texture);

SRE_CAPI_END

#endif