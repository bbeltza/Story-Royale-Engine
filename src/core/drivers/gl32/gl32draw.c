#include "gl32.h"

void sregl32_flush_queueinstances1(void* _inst, void* _texture, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags, sre_u32 switch_flags)
{
    sregl32_inst* inst = _inst;
    sregl_texture* texture = _texture;

    if (switch_flags & SRE_RENDER_SWITCHTYPE)
    {
        SRE_GLCALL(inst->glfuncs21.UseProgram(inst->d1data.program));
        SRE_GLCALL(inst->glfuncs32.BindVertexArray(inst->d1data.vao));
    }
    
    if (switch_flags & SRE_RENDER_SWITCHCAMERA)
    {
        bool switchcam = flags & SRE_DRAWFLAG_CAMERA;
        SRE_GLCALL(inst->glfuncs32.BindBufferRange(GL_UNIFORM_BUFFER, 0, inst->stateubo, sizeof(struct sregl32_stateubo)*switchcam, sizeof(struct sregl32_stateubo)));
    }

    if (switch_flags & SRE_RENDER_SWITCHTEXTURE)
    {
        SRE_GLCALL(inst->glfuncs.BindTexture(GL_TEXTURE_2D, !texture ? inst->common21.basic_texture : texture->gltex));
    }
    
    
    do {
        unsigned colors[4*255];
        GLsizeiptr max_count = instance_count & 255;

        for (size_t i = 0; i < max_count; i++)
        {
            sre_col4 packed = instances[i].color;
            colors[4*i + 0] = packed.r;
            colors[4*i + 1] = packed.g;
            colors[4*i + 2] = packed.b;
            colors[4*i + 3] = packed.a;
        }
        SRE_GLCALL(inst->glfuncs21.BindBuffer(GL_UNIFORM_BUFFER, inst->d1data.instcolubo));
        SRE_GLCALL(inst->glfuncs21.BufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(int[4])*max_count, colors));

        SRE_GLCALL(inst->glfuncs21.BindBuffer(GL_UNIFORM_BUFFER, inst->d1data.instubo));
        SRE_GLCALL(inst->glfuncs21.BufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(sre_RenderInstance1)*max_count, instances));
        SRE_GLCALL(inst->glfuncs32.DrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)max_count));

        instance_count -= max_count;
    } while (instance_count);
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