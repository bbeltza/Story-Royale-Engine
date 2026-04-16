#include "gl21.h"

void sregl21_present(void* _inst)
{
    sregl21_inst* inst = _inst;
    SDL_GL_SwapWindow(inst->window);
    
    SRE_GLCTXEND();
}

bool sregl21_clear(void* _inst, float color[3])
{
    sregl21_inst* inst = _inst;
    SRE_GLCTXMAKE(false);

    SRE_GLCALLF(inst->glfuncs.ClearColor(color[0], color[1], color[2], 1));
    SRE_GLCALLF(inst->glfuncs.Clear(GL_COLOR_BUFFER_BIT));

    return true;
}

static GLfloat NO_CAM[2];

void sregl21_flush_queueinstances1(void* _inst, void* _texture, const sre_RenderInstance1* instances, size_t instance_count, sre_u32 flags, sre_u32 switch_flags)
{
    sregl21_inst* inst = _inst;
    sregl21_texture* texture = _texture;
    SRE_GLCTXCHECK;

    if (switch_flags & SRE_RENDER_SWITCHTYPE)
    {
        switch_flags |= SRE_RENDER_SWITCHTEXTURE | SRE_RENDER_SWITCHCAMERA; // Have to switch everything if there's a type check in here!
        sregl21bindbuffer(inst, inst->draw1data.vbo);
        SRE_GLCALL(inst->glfuncs21.UseProgram(inst->draw1data.program));
        SRE_GLCALL(inst->glfuncs21.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, inst->draw1data.ibo));
    }
    
    if (switch_flags & SRE_RENDER_SWITCHCAMERA)
    {
        bool usecam = flags & SRE_DRAWFLAG_CAMERA;
        SRE_GLCALL(inst->glfuncs21.Uniform2fv(inst->draw1data.common_uniforms.camera, 1, usecam ? inst->cache.camera : NO_CAM));
    }

    if (switch_flags & SRE_RENDER_SWITCHTEXTURE)
    {
        SRE_GLCALL(inst->glfuncs.BindTexture(GL_TEXTURE_2D, !texture ? inst->basic_texture : texture->gltex));
    }

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

void sregl21_flush_queueinstances2(void* _inst, const sre_RenderInstance2* instance, size_t point_count, sre_u32 flags, sre_u32 switch_flags)
{
    sregl21_inst* inst = _inst;
    SRE_GLCTXCHECK;

    if (switch_flags & SRE_RENDER_SWITCHTYPE)
    {
        switch_flags |= /*SRE_RENDER_SWITCHTEXTURE |*/ SRE_RENDER_SWITCHCAMERA; // Have to switch everything if there's a type check in here!
        sregl21bindbuffer(inst, inst->draw2data.vbo);
        SRE_GLCALL(inst->glfuncs21.UseProgram(inst->draw2data.program));
    }
    
    if (switch_flags & SRE_RENDER_SWITCHCAMERA)
    {
        bool usecam = flags & SRE_DRAWFLAG_CAMERA;
        SRE_GLCALL(inst->glfuncs21.Uniform2fv(inst->draw2data.common_uniforms.camera, 1, usecam ? inst->cache.camera : NO_CAM));
    }

    /*
    if (switch_flags & SRE_RENDER_SWITCHTEXTURE)
    {
        SRE_GLCALL(inst->glfuncs.BindTexture(GL_TEXTURE_2D, !texture ? inst->basic_texture : texture->gltex));
    }
        */

    SRE_GLCALL(inst->glfuncs21.Uniform4i(inst->draw2data.common_uniforms.color, instance->color.r, instance->color.g, instance->color.b, instance->color.a));
    if ((GLsizei)point_count > inst->draw2data.bufsize)
    {
        inst->draw2data.bufsize *= 2;
        SRE_GLCALL(inst->glfuncs21.BufferData(GL_ARRAY_BUFFER, inst->draw2data.bufsize*2*sizeof(GLfloat), instance->points, GL_DYNAMIC_DRAW));
    }
    else
    {
        SRE_GLCALL(inst->glfuncs21.BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(instance->points[0])*point_count, instance->points));
    }

    GLenum mode;
    switch (instance->mode)
    {
        case SRE_DRAW2_JOINED: mode = GL_TRIANGLE_FAN; break;
        case SRE_DRAW2_STRIP: mode = GL_TRIANGLE_STRIP; break;
        case SRE_DRAW2_TRIANGLE: mode = GL_TRIANGLES; break;
        default: assert(0);
    }

    SRE_GLCALL(inst->glfuncs.DrawArrays(mode, 0, (GLsizei)point_count));
}