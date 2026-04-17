#include "gl32.h"

void sregl32_flush_queueinstances1(void* _inst, void* texture, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags, sre_u32 switch_flags)
{
    sregl32_inst* inst = _inst;
    
    SRE_GLCALL(inst->glfuncs32.glDrawArraysInstanced(GL_QUADS, 0, 4, (GLsizei)instance_count));
}

void sregl32_flush_queueinstances2(void* inst, void* texture, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags, sre_u32 switch_flags)
{

}

void sregl32_present(void* _inst)
{
    sregl32_inst* inst = _inst;
    SREGL_PRESENT(inst->common21.common);
}

bool sregl32_clear(void* _inst, float color[3])
{
    sregl32_inst* inst = _inst;
    SREGL_CLEAR(inst->glfuncs, color);
}