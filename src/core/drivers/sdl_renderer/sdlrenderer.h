#ifndef SRE_SDLRENDERER_H
#define SRE_SDLRENDERER_H

#include <Core/Render.h>
#include <SDL_render.h>

struct sre_Sampler
{
    SDL_Texture* texture;
};

typedef struct sresdlrenderer_interface
{
    sre_RenderInterface inter;

    SDL_Renderer* renderer;
} sresdlrenderer_interface;

SRE_CAPI_BEGIN

void sresdlrenderer_flush_queueinstances1(sresdlrenderer_interface* inst, sre_Sampler*const* inst_textures, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags);
void sresdlrenderer_flush_queueinstances2(sresdlrenderer_interface* inst, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags);

void sresdlrenderer_present(sresdlrenderer_interface* inst);
bool sresdlrenderer_clear(sresdlrenderer_interface* inst, float color[3]);

bool sresdlrenderer_set_viewportstate(sresdlrenderer_interface* inst, int w, int h, sre_unit scale);
bool sresdlrenderer_set_blendstate(sresdlrenderer_interface* inst, sre_DrawBlending blending);
bool sresdlrenderer_set_camerastate(sresdlrenderer_interface* inst, sre_unit x, sre_unit y);

bool sresdlrenderer_setup_texture(sresdlrenderer_interface* inst, sre_Sampler* texture, sre_pixelFormat format, int w, int h);
bool sresdlrenderer_update_texture(sresdlrenderer_interface* inst, sre_Sampler* texture, const void* pixels, int pitch);
bool sresdlrenderer_query_texture(sresdlrenderer_interface* inst, sre_Sampler* texture, int size[2], sre_pixelFormat* format);

SRE_CAPI_END

#endif