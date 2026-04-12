#include "gl21.h"

void sregl21_present(sregl21_inst* inst)
{
    SDL_GL_SwapWindow(inst->window);    
    SDL_GL_MakeCurrent(inst->window, inst->context);
}

bool sregl21_clear(sregl21_inst* inst, float color[3])
{
    if (SDL_GL_MakeCurrent(inst->window, inst->context))
        return false;

    SRE_GLCALL(glClearColor(color[0], color[1], color[2], 1));
    SRE_GLCALL(glClear(GL_COLOR_BUFFER_BIT));

    return true;
}

void sregl21_flush_queueinstances1(sregl21_inst* inst, sre_Sampler*const* inst_textures, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags)
{
    assert(SDL_GL_GetCurrentContext() == inst->context);
}

void sregl21_flush_queueinstances2(sregl21_inst* inst, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags)
{
    assert(SDL_GL_GetCurrentContext() == inst->context);
}