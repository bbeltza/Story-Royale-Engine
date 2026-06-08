#include "gl21.h"
#include <stdlib.h>

void sregl21_set_viewportstate(void* _inst, int w, int h, sre_unit scale)
{
    sregl21_inst* inst = _inst;
    SRE_GLCALL(inst->glfuncs.Viewport(0, 0, w, h));

    GLfloat VIEWPORT[4*4] = {
        (2.0f/w), 0.0f, 0.0f, 0.0f,
        0.0f, (-2.0f/h), 0.0f, 0.0f,
        0.0f, 0.0f, scale, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f
    };

    SRE_GLCALL(inst->glfuncs21.UniformMatrix4fv(inst->uniforms.viewport, 1, GL_FALSE, VIEWPORT));
}

void sregl21_set_texturestate(void* _inst, void* _texture)
{
    sregl21_inst* inst = _inst;
    sregl_texture* texture = _texture;

    SRE_GLCALL(inst->glfuncs.BindTexture(GL_TEXTURE_2D, !texture ? inst->common21.basic_texture : texture->gltex));
}

void sregl21_set_camerastate(void* _inst, sre_unit x, sre_unit y)
{
    sregl21_inst* inst = _inst;
    
    SRE_GLCALL(inst->glfuncs21.Uniform2f(inst->uniforms.camera, x, y));
}

void sregl21_set_blendstate(void* _inst, sre_blendMode blendmode)
{
    sregl21_inst* inst = _inst;
    sregl_set_blendstate(&inst->glfuncs, blendmode);
}

void sregl21_set_scissorstate(void* _inst, const sre_rect2Di* rectangle)
{
    sregl21_inst* inst = _inst;
    sregl_set_scissorstate(&inst->glfuncs, inst->common21.common.window, rectangle);
}

void sregl21_set_vsync(void* _inst, bool enable)
{
    (void)_inst;
    sregl_set_vsync(enable);
}