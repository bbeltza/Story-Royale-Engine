#include "gl21.h"
#include <stdlib.h>

bool sregl21_set_viewportstate(void* _inst, int w, int h, sre_unit scale)
{
    sregl21_inst* inst = _inst;
    SRE_GLCTXMAKE(false);

    SRE_GLCALLF(inst->glfuncs.Viewport(0, 0, w, h));

    GLfloat VIEWPORT[4*4] = {
        (2.0f/w), 0.0f, 0.0f, 0.0f,
        0.0f, (-2.0f/h), 0.0f, 0.0f,
        0.0f, 0.0f, scale, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f
    };

    SRE_GLCALLF(inst->glfuncs21.UseProgram(inst->draw1data.program));
    SRE_GLCALLF(inst->glfuncs21.UniformMatrix4fv(inst->draw1data.common_uniforms.viewport, 1, GL_FALSE, VIEWPORT));
    SRE_GLCALLF(inst->glfuncs21.UseProgram(inst->draw2data.program));
    SRE_GLCALLF(inst->glfuncs21.UniformMatrix4fv(inst->draw2data.common_uniforms.viewport, 1, GL_FALSE, VIEWPORT));
    return true;
}

bool sregl21_set_blendstate(void* _inst, sre_blendMode blending)
{
    sregl21_inst* inst = _inst;
    SRE_GLCTXCHECK;

    if (blending == SRE_BLEND_NONE)
    {
        SRE_GLCALLF(inst->glfuncs.Disable(GL_BLEND));
        return true;
    }

    GLenum sfactor;
    GLenum dfactor;
    switch (blending)
    {
        case SRE_BLEND_BLEND:
            sfactor = GL_SRC_ALPHA;
            dfactor = GL_ONE_MINUS_SRC_ALPHA;
            break;
        case SRE_BLEND_ADD:
            sfactor = GL_SRC_ALPHA;
            dfactor = GL_ONE;
            break;
        case SRE_BLEND_MOD:
            sfactor = GL_DST_COLOR;
            dfactor = GL_ZERO;
            break;
        case SRE_BLEND_MUL:
            sfactor = GL_DST_COLOR;
            dfactor = GL_ONE_MINUS_SRC_ALPHA;
            break;
        default: abort(); return false;
    }

    SRE_GLCALLF(inst->glfuncs.Enable(GL_BLEND));
    SRE_GLCALLF(inst->glfuncs.BlendFunc(sfactor, dfactor));
    return true;

    return true;
}

bool sregl21_set_camerastate(void* _inst, sre_unit x, sre_unit y)
{
    sregl21_inst* inst = _inst;
    SRE_GLCTXCHECK;

    inst->cache.camera[0] = x;
    inst->cache.camera[1] = y;

    return true;
}

void sregl21_set_clipstate(void* _inst, const sre_rect2Di* rectangle)
{
    return; // clipping is not set up correctly

    sregl21_inst* inst = _inst;
    SRE_GLCTXCHECK;

    int w, h;
    SDL_GL_GetDrawableSize(inst->window, &w, &h);

    if (rectangle)
    {
        SRE_GLCALL(inst->glfuncs.Scissor(
            rectangle->x,
            h - rectangle->y + rectangle->h,
            rectangle->w,
            rectangle->h
        ));
    }
    else
    {
        SRE_GLCALL(inst->glfuncs.Scissor(0, 0, w, h));
    }
}

void sregl21_set_vsync(void* _inst, bool enable)
{
    sregl21_inst* inst = _inst;
    SRE_GLCTXCHECK;

    if (!enable) goto DISABLE;

    if (SDL_GL_SetSwapInterval(-1) != 0)
        SDL_GL_SetSwapInterval(1);
    return;
    
    DISABLE:
        SDL_GL_SetSwapInterval(0);
}