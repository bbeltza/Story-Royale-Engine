#include "gl32.h"

#define SREGL32_CHECK_FORSWITCHES(ddata)    if (switch_flags & SRE_RENDER_SWITCHTYPE)                                                                                                                           \
                                            {                                                                                                                                                                   \
                                                switch_flags |= SRE_RENDER_SWITCHCAMERA | SRE_RENDER_SWITCHTEXTURE;                                                                                             \
                                                SRE_GLCALL(inst->glfuncs21.UseProgram(inst->ddata.program));                                                                                                   \
                                                SRE_GLCALL(inst->glfuncs32.BindVertexArray(inst->ddata.vao));                                                                                                  \
                                                SRE_GLCALL(inst->glfuncs21.BindBuffer(GL_ARRAY_BUFFER, inst->ddata.vbo));                                                                                                  \
                                            }                                                                                                                                                                   \
                                                                                                                                                                                                                \
                                            if (switch_flags & SRE_RENDER_SWITCHCAMERA)                                                                                                                         \
                                            {                                                                                                                                                                   \
                                                bool switchcam = flags & SRE_DRAWFLAG_CAMERA;                                                                                                                   \
                                                GLfloat camera[2] = { (flags & SRE_DRAWFLAG_CAMERAX) ? inst->cache.camera_x : 0, (flags & SRE_DRAWFLAG_CAMERAY) ? inst->cache.camera_y : 0 };                   \
                                                SRE_GLCALL(inst->glfuncs21.BindBuffer(GL_UNIFORM_BUFFER, inst->stateubo));                                                                                      \
                                                SRE_GLCALL(inst->glfuncs21.BufferSubData(GL_UNIFORM_BUFFER, offsetof(struct sregl32_stateubo, camera), sizeof(camera), camera));                                \
                                                SRE_GLCALL(inst->glfuncs21.BindBuffer(GL_UNIFORM_BUFFER, 0));                                                                                                   \
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

    GLsizeiptr inst_counti = (GLsizeiptr)instance_count;
    if (inst->d1data.vbosize < inst_counti)
    {
        inst->d1data.vbosize = inst_counti*2;
        SRE_GLCALL(inst->glfuncs21.BufferData(GL_ARRAY_BUFFER, sizeof(sre_RenderInstance1)*inst->d1data.vbosize, NULL, GL_DYNAMIC_DRAW));
    }
    
    SRE_GLCALL(inst->glfuncs21.BufferData(GL_ARRAY_BUFFER, sizeof(sre_RenderInstance1)*inst_counti, instances, GL_DYNAMIC_DRAW));
    SRE_GLCALL(inst->glfuncs.DrawArrays(GL_POINTS, 0, (GLsizei)instance_count));
}

void sregl32_flush_queueinstances2(void* _inst, void* _texture, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags, sre_u32 switch_flags)
{
    sregl32_inst* inst = _inst;
    sregl_texture* texture = _texture;

    SREGL32_CHECK_FORSWITCHES(d2data);

    GLenum mode = sregl_mapmode(instance->mode);
    GLsizeiptr point_counti = (GLsizeiptr)point_count;
    if (inst->d2data.vbosize < point_counti)
    {
        inst->d2data.vbosize = point_counti*2;
        SRE_GLCALL(inst->glfuncs21.BufferData(GL_ARRAY_BUFFER, sizeof(instance->points[0])*inst->d2data.vbosize, NULL, GL_DYNAMIC_DRAW));
    }

    SRE_GLCALL(inst->glfuncs21.BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(instance->points[0])*point_counti, instance->points));
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
    return true;
}