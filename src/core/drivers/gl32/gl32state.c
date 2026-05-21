#include "gl32.h"

bool sregl32_set_blendstate(void* _inst, sre_blendMode blendmode)
{
    sregl32_inst* inst = _inst;
    return sregl_set_blendstate(&inst->glfuncs, blendmode);
}

void sregl32_set_clipstate(void* _inst, const sre_rect2Di* rectangle)
{
    sregl32_inst* inst = _inst;
    sregl_set_clipstate(&inst->glfuncs, inst->common21.common.window, rectangle);
}

void sregl32_set_vsync(void* _inst, bool enable)
{
    (void)_inst;
    sregl_set_vsync(enable);
}

bool sregl32_set_viewportstate(void* _inst, int w, int h, sre_unit scale)
{
    sregl32_inst* inst = _inst;
    SRE_GLCALLF(inst->glfuncs.Viewport(0, 0, w, h));

    GLfloat proj[16] = {
        2.0f/w, 0.0f, 0.0f, 0.0f,  
        0.0f, -2.0f/h, 0.0f, 0.0f,  
        0.0f, 0.0f, scale, 0.0f,  
        -1.0f, 1.0f, 0.0f, 1.0f,  
    };

    SRE_GLCALLF(inst->glfuncs21.BindBuffer(GL_UNIFORM_BUFFER, inst->stateubo));
    SRE_GLCALLF(inst->glfuncs21.BufferSubData(GL_UNIFORM_BUFFER, offsetof(struct sregl32_stateubo, viewport), sizeof(proj), proj));
    SRE_GLCALLF(inst->glfuncs21.BindBuffer(GL_UNIFORM_BUFFER, 0));
    return true;
}

bool sregl32_set_camerastate(void* _inst, sre_unit x, sre_unit y)
{
    sregl32_inst* inst = _inst;
    inst->cache.camera_x = x;
    inst->cache.camera_y = y;
    return true;
}