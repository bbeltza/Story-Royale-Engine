#include "gl21.h"

bool sregl21_texture_setup(void* _inst, void* _texture, sre_SDLpixelFormat format, int w, int h, sre_SDLpixelFormat* outformat)
{
    sregl21_inst* inst = _inst;
    return sregl_texture_setup(&inst->glfuncs, _texture, format, w, h, outformat);
}

bool sregl21_texture_update(void* _inst, void* _texture, const sre_rect2Di* region, const void* pixels, int pitch)
{
    sregl21_inst* inst = _inst;
    return sregl_texture_update(&inst->glfuncs, _texture, region, pixels, pitch);
}

void sregl21_texture_destroy(void* _inst, void* _texture)
{
    sregl21_inst* inst = _inst;
    sregl_texture_destroy(&inst->glfuncs, _texture);
}