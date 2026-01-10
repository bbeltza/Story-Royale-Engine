#include "sdl_renderer.h"

extern int sresdlrenderer_tex_gen(const sre_videodriver* video, void* texture)
{
    // No action needed for now
    *(SDL_Texture**)texture = NULL;
    return 0;
}
extern int sresdlrenderer_tex_update(const sre_videodriver* video, void* texture, const void* pixels, int pitch)
{
    SDL_Texture** sdltex = texture;
    if (!*sdltex) return -1;

    return SDL_UpdateTexture(*sdltex, NULL, pixels, pitch);
}
extern int sresdlrenderer_tex_bind(const sre_videodriver* video, void* texture, const SDL_Surface* surface)
{
    SDL_Texture** sdltex = texture;
    if (*sdltex) return -1;

    *sdltex = SDL_CreateTextureFromSurface(video->userdata, (SDL_Surface*)surface);
    return *sdltex ? 0 : -1;
}

extern SDL_PixelFormatEnum sresdlrenderer_tex_format(const sre_videodriver* video, void* texture)
{
    SDL_Texture** sdltex = texture;
    if (!*sdltex) return -1;

    Uint32 format;
    if (SDL_QueryTexture(*sdltex, &format, NULL, NULL, NULL) < 0) return SDL_PIXELFORMAT_UNKNOWN;

    return format;
}

extern int sresdlrenderer_tex_size(const sre_videodriver* video, void* texture, int* w, int* h)
{
    SDL_Texture** sdltex = texture;
    if (!*sdltex) return -1;

    return SDL_QueryTexture(*sdltex, NULL, NULL, w, h);
}

extern void sresdlrenderer_tex_destroy(const sre_videodriver* video, void* texture)
{
    SDL_Texture** sdltex = texture;
    if (!*sdltex) return;

    SDL_DestroyTexture(*sdltex);
}