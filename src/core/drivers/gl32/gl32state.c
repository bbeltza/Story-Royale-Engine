#include "gl32.h"

bool sregl32_set_blendstate(void* inst, sre_blendMode blending)
{

}

void sregl32_set_clipstate(void* inst, const sre_rect2Di* rectangle)
{

}

void sregl32_set_vsync(void* inst, bool enable)
{
    
}

bool sregl32_set_viewportstate(void* _inst, int w, int h, sre_unit scale)
{
    sregl32_inst* inst = _inst;
    SRE_GLCALLF(inst->glfuncs.Viewport(0, 0, w, h));

    return true;
}

bool sregl32_set_camerastate(void* inst, sre_unit x, sre_unit y)
{

}