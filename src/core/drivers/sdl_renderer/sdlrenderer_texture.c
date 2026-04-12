#include "sdlrenderer.h"

bool sresdlrenderer_setup_texture(void* _inst, sre_Sampler* texture, sre_pixelFormat format, int w, int h)
{
    sresdlrenderer_interface* inst = _inst;

    texture->texture = SDL_CreateTexture(inst->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, w, h);
    SDL_SetTextureBlendMode(texture->texture, SDL_BLENDMODE_BLEND);
    
    return texture->texture != NULL;
}
bool sresdlrenderer_update_texture(void* _inst, sre_Sampler* texture, const void* pixels, int pitch)
{
    sresdlrenderer_interface* inst = _inst;

    return SDL_UpdateTexture(texture->texture, NULL, pixels, pitch) == 0;
}

bool sresdlrenderer_query_texture(void* _inst, sre_Sampler* texture, int size[2], sre_pixelFormat* format)
{
    sresdlrenderer_interface* inst = _inst;

    return SDL_QueryTexture(texture->texture, (Uint32*)format, NULL, &size[0], size ? &size[1] : NULL) == 0;
}

void sresdlrenderer_destroy_texture(void* _inst, sre_Sampler* texture)
{
    sresdlrenderer_interface* inst = _inst;

    SDL_DestroyTexture(texture->texture);
}