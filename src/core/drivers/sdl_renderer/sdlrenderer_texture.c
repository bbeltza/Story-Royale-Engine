#include "sdlrenderer.h"

bool sresdlrenderer_setup_texture(void* _inst, void* _texture, sre_SDLpixelFormat format, int w, int h, sre_SDLpixelFormat* outformat)
{
    sresdlrenderer_inst* inst = _inst;
    sresdlrenderer_texture* texture = _texture;

    texture->texture = SDL_CreateTexture(inst->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, w, h);
    SDL_SetTextureBlendMode(texture->texture, SDL_BLENDMODE_BLEND);
    
    *outformat = SDL_PIXELFORMAT_RGBA32;
    return texture->texture != NULL;
}
bool sresdlrenderer_update_texture(void* _inst, void* _texture, const sre_rect2Di* region, const void* pixels, int pitch)
{
    (void)_inst;
    sresdlrenderer_texture* texture = _texture;

    return SDL_UpdateTexture(texture->texture, &(SDL_Rect){ region->x, region->y, region->w, region->h }, pixels, pitch) == 0;
}

void sresdlrenderer_destroy_texture(void* _inst, void* _texture)
{
    (void)_inst;
    sresdlrenderer_texture* texture = _texture;

    SDL_DestroyTexture(texture->texture);
}