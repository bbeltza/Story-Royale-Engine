#include "gl32.h"

void sregl32_set_blendstate(void* _inst, sre_blendMode blendmode)
{
    sregl32_inst* inst = _inst;
    sregl_set_blendstate(&inst->glfuncs, blendmode);
}

void sregl32_set_scissorstate(void* _inst, const sre_rect2Di* rectangle)
{
    sregl32_inst* inst = _inst;
    sregl_set_scissorstate(&inst->glfuncs, inst->common21.common.window, rectangle);
}

void sregl32_set_vsync(void* _inst, bool enable)
{
    (void)_inst;
    sregl_set_vsync(enable);
}

void sregl32_set_viewportstate(void* _inst, const sre_rect2Di* rectangle, sre_unit scale)
{
    sregl32_inst* inst = _inst;
    sregl_set_viewportstate(&inst->glfuncs, inst->common21.common.window, rectangle);

    GLfloat proj[16] = {
        2.0f/rectangle->w, 0.0f, 0.0f, 0.0f,  
        0.0f, -2.0f/rectangle->h, 0.0f, 0.0f,  
        0.0f, 0.0f, scale, 0.0f,  
        -1.0f, 1.0f, 0.0f, 1.0f,  
    };

    SRE_GLCALL(inst->glfuncs21.BindBuffer(GL_UNIFORM_BUFFER, inst->stateubo));
    SRE_GLCALL(inst->glfuncs21.BufferSubData(GL_UNIFORM_BUFFER, offsetof(struct sregl32_stateubo, viewport), sizeof(proj), proj));
    SRE_GLCALL(inst->glfuncs21.BindBuffer(GL_UNIFORM_BUFFER, 0));
}

void sregl32_set_camerastate(void* _inst, sre_unit x, sre_unit y)
{
    sregl32_inst* inst = _inst;
    GLfloat cam[2] = { x, y };
    SRE_GLCALL(inst->glfuncs21.BindBuffer(GL_UNIFORM_BUFFER, inst->stateubo));
    SRE_GLCALL(inst->glfuncs21.BufferSubData(GL_UNIFORM_BUFFER, offsetof(struct sregl32_stateubo, camera), sizeof(cam), cam));
    SRE_GLCALL(inst->glfuncs21.BindBuffer(GL_UNIFORM_BUFFER, 0));
}

void sregl32_set_texturestate(void* _inst, void* _texture)
{
    sregl32_inst* inst = _inst;
    sregl_texture* texture = _texture;

    SRE_GLCALL(inst->glfuncs.BindTexture(GL_TEXTURE_2D, texture ? texture->gltex : inst->common21.basic_texture));
}