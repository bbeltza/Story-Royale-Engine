#include "gl32.h"

bool sregl32_texture_setup(void* _inst, void* texture, sre_pixelFormat format, int w, int h, sre_pixelFormat* outformat)
{
    sregl32_inst* inst = _inst;
    return sregl_texture_setup(&inst->glfuncs, texture, format, w, h, outformat);
}

bool sregl32_texture_update(void* _inst, void* texture, const void* pixels, int pitch)
{
    sregl32_inst* inst = _inst;
    return sregl_texture_update(&inst->glfuncs, texture, pixels, pitch);
}

void sregl32_texture_destroy(void* _inst, void* texture)
{
    sregl32_inst* inst = _inst;
    sregl_texture_destroy(&inst->glfuncs, texture);
}