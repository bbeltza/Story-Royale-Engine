#include "sdl_renderer.h"

extern bool sresdlrenderer_tex_gen(const sre_videodriver* video, void* texture)
{
    // No action needed for now
    *(SDL_Texture**)texture = NULL;
    return true;
}
extern bool sresdlrenderer_tex_update(const sre_videodriver* video, void* texture, const void* pixels, int pitch)
{
    SDL_Texture** sdltex = texture;
    if (!*sdltex) return false;

    return SDL_UpdateTexture(*sdltex, NULL, pixels, pitch) == 0;
}
extern bool sresdlrenderer_tex_bind(const sre_videodriver* video, void* texture, const SDL_Surface* surface)
{
    SDL_Texture** sdltex = texture;
    if (*sdltex) return false;

    *sdltex = SDL_CreateTextureFromSurface(video->userdata, (SDL_Surface*)surface);
    return *sdltex != NULL;
}

extern SDL_PixelFormatEnum sresdlrenderer_tex_format(const sre_videodriver* video, void* texture)
{
    SDL_Texture** sdltex = texture;
    if (!*sdltex) return SDL_PIXELFORMAT_UNKNOWN;

    Uint32 format;
    if (SDL_QueryTexture(*sdltex, &format, NULL, NULL, NULL) < 0) return SDL_PIXELFORMAT_UNKNOWN;

    return format;
}

extern bool sresdlrenderer_tex_size(const sre_videodriver* video, void* texture, int* w, int* h)
{
    SDL_Texture** sdltex = texture;
    if (!*sdltex) return false;

    return SDL_QueryTexture(*sdltex, NULL, NULL, w, h) == 0;
}

extern void sresdlrenderer_tex_destroy(const sre_videodriver* video, void* texture)
{
    SDL_Texture** sdltex = texture;
    if (!*sdltex) return;

    SDL_DestroyTexture(*sdltex);
}