#include "gl32.h"

#define SREGL32_CHECK_FORSWITCHES(ddata)    if (switch_flags & SRE_RENDER_SWITCHTYPE)                                                                                                                           \
                                            {                                                                                                                                                                   \
                                                switch_flags |= SRE_RENDER_SWITCHCAMERA | SRE_RENDER_SWITCHTEXTURE;                                                                                             \
                                                SRE_GLCALL(inst->glfuncs21.UseProgram(inst->ddata.program));                                                                                                   \
                                                SRE_GLCALL(inst->glfuncs32.BindVertexArray(inst->ddata.vao));                                                                                                  \
                                            }                                                                                                                                                                   \
                                                                                                                                                                                                                \
                                            if (switch_flags & SRE_RENDER_SWITCHCAMERA)                                                                                                                         \
                                            {                                                                                                                                                                   \
                                                bool switchcam = flags & SRE_DRAWFLAG_CAMERA;                                                                                                                   \
                                                SRE_GLCALL(inst->glfuncs32.BindBufferRange(GL_UNIFORM_BUFFER, 0, inst->stateubo, sizeof(struct sregl32_stateubo)*switchcam, sizeof(struct sregl32_stateubo)));  \
                                            }                                                                                                                                                                   \
                                                                                                                                                                                                                \
                                            if (switch_flags & SRE_RENDER_SWITCHTEXTURE)                                                                                                                        \
                                            {                                                                                                                                                                   \
                                                SRE_GLCALL(inst->glfuncs.BindTexture(GL_TEXTURE_2D, !texture ? inst->common21.basic_texture : texture->gltex));                                                 \
                                            }                                                                                                                                                                   \

void sregl32_flush_queueinstances1(void* _inst, void* _texture, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags, sre_u32 switch_flags)
{
    sregl32_inst* inst = _inst;
    sregl_texture* texture = _texture;

    SREGL32_CHECK_FORSWITCHES(d1data);
    
    while (instance_count)
    {
        unsigned colors[4*255];
        GLsizeiptr max_count = instance_count > 255 ? 255 : instance_count;

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
        instances += max_count;
    } 
}

void sregl32_flush_queueinstances2(void* _inst, void* _texture, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags, sre_u32 switch_flags)
{
    sregl32_inst* inst = _inst;
    sregl_texture* texture = _texture;

    SREGL32_CHECK_FORSWITCHES(d2data);

    GLenum mode;
    switch (instance->mode)
    {
        case SRE_DRAW2_JOINED: mode = GL_TRIANGLE_FAN; break;
        case SRE_DRAW2_STRIP: mode = GL_TRIANGLE_STRIP; break;
        case SRE_DRAW2_TRIANGLE: mode = GL_TRIANGLES; break;
        default: abort(); return;
    }

    if (inst->d2data.vbosize < point_count)
    {
        inst->d2data.vbosize *= 2;
        SRE_GLCALL(inst->glfuncs21.BufferData(GL_ARRAY_BUFFER, sizeof(instance->points[0])*inst->d2data.vbosize, instance->points, GL_DYNAMIC_DRAW));
    }
    else
    {
        SRE_GLCALL(inst->glfuncs21.BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(instance->points[0])*point_count, instance->points));
    }
    SRE_GLCALL(inst->glfuncs21.Uniform4i(inst->d2data.coluniform, instance->color.r, instance->color.g, instance->color.b, instance->color.a));
    SRE_GLCALL(inst->glfuncs.DrawArrays(mode, 0, (GLsizei)point_count));
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