#ifndef SRE_SDLRENDERER_H
#define SRE_SDLRENDERER_H

#include <Core/Render.h>
#include <SDL_render.h>

union sresdlrenderer_iprim
{
    struct
    {
        unsigned l0;
        unsigned l1;
        unsigned l2;
        unsigned r0;
        unsigned r1;
        unsigned r2;
    };
    unsigned i[6];    
};

typedef struct sresdlrenderer_texture
{
    SDL_Texture* texture;
} sresdlrenderer_texture;

typedef struct sresdlrenderer_inst
{
    SDL_Renderer* renderer;
    SDL_Texture* cur_texture;

    sre_unit scaling;
    sre_vec2ut camera;
    sre_vec2ut usecamera;

    SDL_Vertex* vbuf;
    size_t vbuf_size;

    union sresdlrenderer_iprim* ibuf;
    size_t ibuf_size;
} sresdlrenderer_inst;

SRE_CAPI_BEGIN

void sresdlrenderer_draw1(void* _inst, const sre_RenderInstance1* instances, size_t instance_count);
void sresdlrenderer_draw2(void* _inst, const sre_RenderPoint* points, size_t point_count, sre_draw2primitive mode);

void sresdlrenderer_begin(void* _inst, const float clear[4]);
void sresdlrenderer_end(void* _inst);

void sresdlrenderer_set_viewportstate(void* _inst, int w, int h, sre_unit scale);
void sresdlrenderer_set_vsync(void* _inst, bool enable);
void sresdlrenderer_set_texturestate(void* _inst, void* _texture);
void sresdlrenderer_set_blendstate(void* _inst, sre_blendMode blending);
void sresdlrenderer_set_camerastate(void* _inst, sre_unit x, sre_unit y);
void sresdlrenderer_set_scissorstate(void* _inst, const sre_rect2Di* rectangle);

bool sresdlrenderer_setup_texture(void* _inst, void* _texture, sre_SDLpixelFormat format, int w, int h, sre_SDLpixelFormat* outformat);
bool sresdlrenderer_update_texture(void* _inst, void* _texture, const sre_rect2Di* region, const void* pixels, int pitch);
void sresdlrenderer_destroy_texture(void* _inst, void* _texture);

SRE_CAPI_END

#endif