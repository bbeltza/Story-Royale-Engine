#include "gl21.h"

static void sregl21_destroy(void* _inst);

static const struct sre_RenderVFT sregl21_vft = {
    sregl21_destroy,
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
    sregl21_destroy_texture
};

static bool sregl21_main(const struct sre_RenderVFT** vft, void* _inst, SDL_Window* window)
{
    sregl21_inst* inst = _inst;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    inst->context = SDL_GL_CreateContext(window);
    if (!inst->context)
        goto CLEAN_NFAIL;

    // Load functions (We're loading every single OpenGL function including the legacy ones that we need, since we don't link against OpenGL statically)

    if (!sregl_loadfunctions(&inst->glfuncs, SDL_GL_GetProcAddress))
        goto CLEAN_NFAIL;
    if (!sregl_loadfunctions21(&inst->glfuncs21, SDL_GL_GetProcAddress))
        goto CLEAN_NFAIL;

    //
    // Setup state
    if (!sregl21setupbuffers(inst))
        goto CLEAN_NFAIL;

    //

    inst->window = window;
    *vft = &sregl21_vft;
    return true;

    CLEAN_NFAIL:
        SDL_GL_DeleteContext(inst->context);
        return false;
}

static void sregl21_destroy(void* _inst)
{
    sregl21_inst* inst = _inst;

    SDL_GL_DeleteContext(inst->context);
}

sre_RenderDriverData sregl21 = {
    .initialize = sregl21_main,
    .renderer_size = sizeof(sregl21_inst),
    .texture_size = sizeof(sregl21_texture)
};