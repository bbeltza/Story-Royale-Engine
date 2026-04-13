#include "gl21.h"

bool sregl21_setup_texture(void* _inst, sre_Sampler* texture, sre_pixelFormat format, int w, int h)
{
    texture->w = w;
    texture->h = h;
    return true;
}

bool sregl21_update_texture(void* _inst, sre_Sampler* texture, const void* pixels, int pitch)
{

    return true;
}

bool sregl21_query_texture(void* _inst, sre_Sampler* texture, int size[2], sre_pixelFormat* format)
{
    if (size)
    {
        size[0] = texture->w;
        size[1] = texture->h;
    }
    if (format)
        *format = SDL_PIXELFORMAT_ABGR8888;

    return true;
}

void sregl21_destroy_texture(void* _inst, sre_Sampler* texture)
{

}