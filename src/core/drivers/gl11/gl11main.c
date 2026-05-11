#include "gl11.h"

static void sregl11_destroy(void* _inst)
{
    sregl11_inst* inst = _inst;
    sregl_commondestroy(&inst->common);
}

static const struct sre_RenderVFT sregl11vft = {
    sregl11_destroy,
    sregl11_flush_queueinstances1,
    sregl11_flush_queueinstances2,
    sregl11_present,
    sregl11_clear,
    sregl11_set_viewportstate,
    sregl11_set_blendstate,
    sregl11_set_camerastate,
    sregl11_set_clipstate,
    sregl11_set_vsync,
    sregl11_texture_setup,
    sregl11_texture_update,
    sregl11_texture_destroy
};

static int sregl11_main(const struct sre_RenderVFT** vft, void* _inst, SDL_Window* window)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    sregl11_inst* inst = _inst;
    int setupstatus = sregl_commonsetup(&inst->common, window, &inst->glfuncs);
    if (setupstatus != SRE_RENDERSTATUS_SUCCEEDED)
        return setupstatus;
    
    if (!sregl_loadfunctions11(&inst->glfuncs11, SDL_GL_GetProcAddress))
    #if 0
        return SRE_RENDERSTATUS_FAILED;
    #else
        return SRE_RENDERSTATUS_UNSUPPORTED;
    #endif

    SRE_GLCALLF(inst->glfuncs.Enable(GL_TEXTURE_2D));

    *vft = &sregl11vft;
    return SRE_RENDERSTATUS_SUCCEEDED;
}

struct sre_RenderDriverData sregl11 = {
    .initialize = sregl11_main,
    .renderer_size = sizeof(sregl11_inst),
    .texture_size = sizeof(sregl11_texture),
    .name = "OpenGL Legacy 1.1"
};