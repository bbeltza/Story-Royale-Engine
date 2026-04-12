#include "gl21.h"

void sregl21_present(void* _inst)
{
    sregl21_inst* inst = _inst;
    SDL_GL_SwapWindow(inst->window);    
    SDL_GL_MakeCurrent(NULL, NULL);
}

bool sregl21_clear(void* _inst, float color[3])
{
    sregl21_inst* inst = _inst;
    SRE_GLCTXMAKE(false);

    SRE_GLCALL(inst->glfuncs.ClearColor(color[0], color[1], color[2], 1));
    SRE_GLCALL(inst->glfuncs.Clear(GL_COLOR_BUFFER_BIT));

    return true;
}

void sregl21_flush_queueinstances1(void* _inst, sre_Sampler*const* inst_textures, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags)
{
    sregl21_inst* inst = _inst;
    SRE_GLCTXCHECK;


}

void sregl21_flush_queueinstances2(void* _inst, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags)
{
    sregl21_inst* inst = _inst;
    SRE_GLCTXCHECK;
}