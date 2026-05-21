#include "sdlrenderer.h"

#include <SDL_hints.h>

static void sresdlrenderer_destroy(void* _inst);
const struct sre_RenderVFT sresdlrenderer_vft = {
    sresdlrenderer_destroy,
    sresdlrenderer_flush_queueinstances1,
    sresdlrenderer_flush_queueinstances2,
    sresdlrenderer_present,
    sresdlrenderer_clear,
    sresdlrenderer_set_viewportstate,
    sresdlrenderer_set_blendstate,
    sresdlrenderer_set_camerastate,
    sresdlrenderer_set_clipstate,
    sresdlrenderer_set_vsync,
    sresdlrenderer_setup_texture,
    sresdlrenderer_update_texture,
    sresdlrenderer_destroy_texture
};

#include <Base/Log.h>
static int sresdlrenderer_main(const struct sre_RenderVFT** vft, void* _inst, SDL_Window* window)
{
    sresdlrenderer_inst* inst = _inst;
    
    inst->renderer = SDL_CreateRenderer(window, -1, 0);
    if (!inst->renderer)
    {
        SDL_free(inst);
        return SRE_RENDERSTATUS_FAILED;
    }
    
    inst->vbuf = NULL;
    inst->vbuf_size = 0;
    inst->ibuf = NULL;
    inst->ibuf_size = 0;

    *vft = &sresdlrenderer_vft;
    return SRE_RENDERSTATUS_SUCCEEDED;
}

static void sresdlrenderer_destroy(void* _inst)
{
    sresdlrenderer_inst* inst = _inst;
    SDL_free(inst->ibuf);
    SDL_free(inst->vbuf);
    SDL_DestroyRenderer(inst->renderer);
}

sre_RenderDriverData sresdlrenderer = {
    .initialize = sresdlrenderer_main,
    .renderer_size = sizeof(sresdlrenderer_inst),
    .texture_size = sizeof(sresdlrenderer_texture),
    .name = "SDL_Renderer"
};