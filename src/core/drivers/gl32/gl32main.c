#include <SDL_video.h>
#include "gl32.h"

static void sregl32_destroy(void* _inst)
{
    sregl32_inst* inst = _inst;
    sregl21_commondestroy(&inst->common21);
}

const struct sre_RenderVFT sregl32_vft = {
    sregl32_destroy,
    sregl32_draw1,
    sregl32_draw2,
    sregl32_begin,
    sregl32_end,
    NULL,
    sregl32_set_vsync,
    sregl32_set_texturestate,
    sregl32_set_blendstate,
    sregl32_set_camerastate,
    sregl32_set_viewportstate,
    sregl32_set_scissorstate,
    sregl32_texture_setup,
    sregl32_texture_update,
    sregl32_texture_destroy
};

static int sregl32_main(const struct sre_RenderVFT** vft, void* _inst, SDL_Window* window)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    sregl32_inst* inst = _inst;
    int setupstatus = sregl21_commonsetup(&inst->common21, window, &inst->glfuncs, &inst->glfuncs21);
    if (setupstatus != SRE_RENDERSTATUS_SUCCEEDED)
        return setupstatus;

    if (!sregl_loadfunctions32(&inst->glfuncs32, SDL_GL_GetProcAddress))
    {
        sregl21_commondestroy(&inst->common21);
        #if 0
            return SRE_RENDERSTATUS_FAILED;
        #else
            return SRE_RENDERSTATUS_UNSUPPORTED;
        #endif
    }

    if (!sregl32setupbuffers(inst))
        return SRE_RENDERSTATUS_FAILED;

    *vft = &sregl32_vft;
    return SRE_RENDERSTATUS_SUCCEEDED;
}

const struct sre_RenderDriverData sregl32 = {
    .initialize = sregl32_main,
    .renderer_size = sizeof(sregl32_inst),
    .texture_size = sizeof(sregl_texture),
    .name = "OpenGL 3.2 Core",
    .flags = SRE_RENDERBIT_SUPPORT_LINELOOP
};