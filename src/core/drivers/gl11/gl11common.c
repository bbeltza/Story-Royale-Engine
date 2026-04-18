#include "gl11.h"

// common drawing

void sregl11_present(void* _inst)
{
    sregl11_inst* inst = _inst;
    SREGL_PRESENT(inst->common);
}

bool sregl11_clear(void* _inst, float color[3])
{
    sregl11_inst* inst = _inst;
    SREGL_CLEAR(inst->glfuncs, color);
    return true;
}

// common set_ functions

bool sregl11_set_blendstate(void* _inst, sre_blendMode blendmode)
{
    sregl11_inst* inst = _inst;
    return sregl_set_blendstate(&inst->glfuncs, blendmode);
}

void sregl11_set_clipstate(void* _inst, const sre_rect2Di* rectangle)
{
    sregl11_inst* inst = _inst;
    sregl_set_clipstate(&inst->glfuncs, inst->common.window, rectangle);
}

void sregl11_set_vsync(void* _inst, bool enable)
{
    (void)_inst;
    sregl_set_vsync(enable);
}

// all of the texture functions

bool sregl11_texture_setup(void* _inst, void* texture, sre_pixelFormat format, int w, int h, sre_pixelFormat* outformat)
{
    sregl11_inst* inst = _inst;
    return sregl_texture_setup(&inst->glfuncs, texture, format, w, h, outformat);
}

bool sregl11_texture_update(void* _inst, void* texture, const void* pixels, int pitch)
{
    sregl11_inst* inst = _inst;
    return sregl_texture_update(&inst->glfuncs, texture, pixels, pitch);
}

void sregl11_texture_destroy(void* _inst, void* texture)
{
    sregl11_inst* inst = _inst;
    sregl_texture_destroy(&inst->glfuncs, texture);
}