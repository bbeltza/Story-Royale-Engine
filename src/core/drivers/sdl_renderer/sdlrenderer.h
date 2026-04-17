#ifndef SRE_SDLRENDERER_H
#define SRE_SDLRENDERER_H

#include <Core/Render.h>
#include <SDL_render.h>

typedef struct sresdlrenderer_texture
{
    SDL_Texture* texture;
} sresdlrenderer_texture;

typedef struct sresdlrenderer_inst
{
    SDL_Renderer* renderer;
    sre_unit scaling;
    sre_vec2ut camera;
} sresdlrenderer_inst;

SRE_CAPI_BEGIN

void sresdlrenderer_flush_queueinstances1(void* inst, void* texture, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags, sre_u32 switch_flags);
void sresdlrenderer_flush_queueinstances2(void* inst, void* texture, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags, sre_u32 switch_flags);

void sresdlrenderer_present(void* inst);
bool sresdlrenderer_clear(void* inst, float color[3]);

bool sresdlrenderer_set_viewportstate(void* inst, int w, int h, sre_unit scale);
bool sresdlrenderer_set_blendstate(void* inst, sre_blendMode blending);
bool sresdlrenderer_set_camerastate(void* inst, sre_unit x, sre_unit y);
void sresdlrenderer_set_clipstate(void* inst, const sre_rect2Di* rectangle);
void sresdlrenderer_set_vsync(void* inst, bool enable);

bool sresdlrenderer_setup_texture(void* inst, void* texture, sre_pixelFormat format, int w, int h, sre_pixelFormat* outformat);
bool sresdlrenderer_update_texture(void* inst, void* texture, const void* pixels, int pitch);
void sresdlrenderer_destroy_texture(void* inst, void* texture);

SRE_CAPI_END

#endif