#include "sdlrenderer.h"

const struct _sre_RenderInterfacevft sresdlrenderer_vft = {
    sresdlrenderer_flush_queueinstances1,
    sresdlrenderer_flush_queueinstances2,
    sresdlrenderer_present,
    sresdlrenderer_clear,
    sresdlrenderer_set_viewportstate,
    sresdlrenderer_set_blendstate,
    sresdlrenderer_set_camerastate,
    sresdlrenderer_setup_texture,
    sresdlrenderer_update_texture,
    sresdlrenderer_query_texture
};

sre_RenderInterface* sresdlrenderer_main(SDL_Window* window)
{
    sresdlrenderer_interface* inst = SDL_calloc(1, sizeof(sresdlrenderer_interface));
    if (!inst)
        return NULL;
    
    inst->inter.vftptr = &sresdlrenderer_vft;
    inst->renderer = SDL_CreateRenderer(window, -1, 0);
    if (!inst->renderer)
    {
        SDL_free(inst);
        return NULL;
    }

    return &inst->inter;
}

void sresdlrenderer_destroy(void* renderer)
{
    sresdlrenderer_interface* inst = renderer;
    SDL_DestroyRenderer(inst->renderer);
    SDL_free(inst->renderer);
}

extern sre_RenderDriverData sresdlrenderer = {
    .init = sresdlrenderer_main,
    .destroy = sresdlrenderer_destroy,
    .texture_size = sizeof(SDL_Texture*)
};