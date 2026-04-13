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

    SRE_GLCALLF(inst->glfuncs.ClearColor(color[0], color[1], color[2], 1));
    SRE_GLCALLF(inst->glfuncs.Clear(GL_COLOR_BUFFER_BIT));

    return true;
}

void sregl21_flush_queueinstances1(void* _inst, sre_Sampler*const* inst_textures, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags)
{
    sregl21_inst* inst = _inst;
    SRE_GLCTXCHECK;

    sregl21bindbuffer(inst, inst->draw1data.vbo);
    SRE_GLCALL(inst->glfuncs21.UseProgram(inst->draw1data.program));
    SRE_GLCALL(inst->glfuncs21.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, inst->draw1data.ibo));

    for (size_t i = 0; i < instance_count; i++)
    {
        const sre_RenderInstance1* dinst = &instances[i];

        GLfloat model[4*3] = {
            dinst->rectangle.x, dinst->rectangle.y, dinst->rectangle.w, dinst->rectangle.h,
            dinst->anchor.x,    dinst->anchor.y,    dinst->angle,       0,
            dinst->uv.x,        dinst->uv.y,        dinst->uv_offset.x, dinst->uv_offset.y
        };

        SRE_GLCALL(inst->glfuncs21.UniformMatrix3x4fv(inst->draw1data.depend_uniforms.model, 1, GL_FALSE, model));
        SRE_GLCALL(inst->glfuncs21.Uniform4i(inst->draw1data.common_uniforms.color, dinst->color.r, dinst->color.g, dinst->color.b, dinst->color.a));
        SRE_GLCALL(inst->glfuncs.DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0));
    }
}

void sregl21_flush_queueinstances2(void* _inst, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags)
{
    sregl21_inst* inst = _inst;
    SRE_GLCTXCHECK;
}