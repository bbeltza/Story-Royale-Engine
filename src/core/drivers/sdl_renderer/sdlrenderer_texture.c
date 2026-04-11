#include "sdlrenderer.h"

bool sresdlrenderer_setup_texture(sresdlrenderer_interface* inst, sre_Sampler* texture, sre_pixelFormat format, int w, int h)
{
    texture->texture = SDL_CreateTexture(inst->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, w, h);
    
    return texture->texture != NULL;
}
bool sresdlrenderer_update_texture(sresdlrenderer_interface* inst, sre_Sampler* texture, const void* pixels, int pitch)
{
    return SDL_UpdateTexture(texture->texture, NULL, pixels, pitch) == 0;
}

bool sresdlrenderer_query_texture(sresdlrenderer_interface* inst, sre_Sampler* texture, int size[2], sre_pixelFormat* format)
{
    return SDL_QueryTexture(texture->texture, (Uint32*)format, NULL, &size[0], size ? &size[1] : NULL) == 0;
}