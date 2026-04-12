#include "gl21.h"

static const struct _sre_RenderInterfacevft sregl21_vft = {
    sregl21_flush_queueinstances1,
    sregl21_flush_queueinstances2,
    sregl21_present,
    sregl21_clear,
    sregl21_set_viewportstate,
    sregl21_set_blendstate,
    sregl21_set_camerastate,
    sregl21_set_clipstate,
    sregl21_set_vsync,
    sregl21_setup_texture,
    sregl21_update_texture,
    sregl21_query_texture,
    sregl21_destroy_texture
};

static sre_RenderInterface* sregl21_main(SDL_Window* window)
{
    sregl21_inst* inst = SDL_calloc(1, sizeof(sregl21_inst));
    if (!inst)
        return NULL;
    sre_RIconstructor(&inst->interface);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    inst->context = SDL_GL_CreateContext(window);
    if (!inst->context)
        goto CLEAN_NFAIL;

    if (!sregl_loadfunctions(&inst->glfuncs, SDL_GL_GetProcAddress))
        goto CLEAN_NFAIL;



    inst->window = window;
    inst->interface.vftptr = &sregl21_vft;
    return &inst->interface;

    CLEAN_NFAIL:
        sre_RIdestructor(&inst->interface);
        SDL_free(inst);
        return NULL;
}

static void sregl21_destroy(sre_RenderInterface* _inst)
{
    sregl21_inst* inst = (sregl21_inst*)_inst;

    SDL_GL_DeleteContext(inst->context);

    sre_RIdestructor(_inst);
    SDL_free(inst);
}

sre_RenderDriverData sregl21 = {
    .init = sregl21_main,
    .destroy = sregl21_destroy,
    .texture_size = sizeof(sre_Sampler)
};