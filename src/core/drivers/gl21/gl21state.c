#include "gl21.h"

bool sregl21_set_viewportstate(sregl21_inst* inst, int w, int h, sre_unit scale)
{
    SRE_GLCALL(glViewport(0, 0, w, h));
    return true;
}

bool sregl21_set_blendstate(sregl21_inst* inst, sre_blendMode blending)
{
    return true;
}

bool sregl21_set_camerastate(sregl21_inst* inst, sre_unit x, sre_unit y)
{
    return true;
}

void sregl21_set_clipstate(sregl21_inst* inst, const sre_rect2Di* rectangle)
{

}

void sregl21_set_vsync(sregl21_inst* inst, bool enable)
{

}