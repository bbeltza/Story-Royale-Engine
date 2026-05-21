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
    sregl21_texture_setup,
    sregl21_texture_update,
    sregl21_texture_destroy
};

static int sregl21_main(const struct sre_RenderVFT** vft, void* _inst, SDL_Window* window)
{
    sregl21_inst* inst = _inst;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    int setupstatus = sregl21_commonsetup(&inst->common21, window, &inst->glfuncs, &inst->glfuncs21);
    if (setupstatus != SRE_RENDERSTATUS_SUCCEEDED)
        return setupstatus;

    //
    // Setup state
    if (!sregl21setupbuffers(inst))
        goto CLEAN_NFAIL;

    //

    *vft = &sregl21_vft;
    return SRE_RENDERSTATUS_SUCCEEDED;

    CLEAN_NFAIL:
        sregl21_commondestroy(&inst->common21);
        return SRE_RENDERSTATUS_FAILED;
}

static void sregl21_destroy(void* _inst)
{
    sregl21_inst* inst = _inst;

    sregl21_commondestroy(&inst->common21);
}

sre_RenderDriverData sregl21 = {
    .initialize = sregl21_main,
    .renderer_size = sizeof(sregl21_inst),
    .texture_size = sizeof(sregl_texture),
    .name = "OpenGL 2.1",
    .flags = SRE_RENDERBIT_SUPPORT_LINELOOP
};

int sregl21_commonsetup(sregl21_cominst* inst, SDL_Window* window, struct sregl_functions* glfuncs, struct sregl_functions21* glfuncs21)
{
    int setupstatus = sregl_commonsetup(&inst->common, window, glfuncs);
    if (setupstatus != SRE_RENDERSTATUS_SUCCEEDED)
        return setupstatus;

    // Load functions (We're loading every single OpenGL function including the legacy ones that we need, since we don't link against OpenGL statically)
    if (!sregl_loadfunctions21(glfuncs21, SDL_GL_GetProcAddress))
    {
        sregl_commondestroy(&inst->common);
        #if 0
            return SRE_RENDERSTATUS_FAILED;
        #else
            return SRE_RENDERSTATUS_UNSUPPORTED;
        #endif
    }

    #undef SRE_GLGETERROR
    #define SRE_GLGETERROR glfuncs->GetError
    { // Basic flat texture needs to be set up in both Open GL 2.1 and 3.2
        int WHITE = -1; // 0xFFFFFFFF
        SRE_GLCALLC(glfuncs->GenTextures(1, &inst->basic_texture));
        SRE_GLCALLC(glfuncs->BindTexture(GL_TEXTURE_2D, inst->basic_texture));
        SRE_GLCALLC(glfuncs->TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &WHITE));
    }
    return SRE_RENDERSTATUS_SUCCEEDED;
}