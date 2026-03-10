#include "sdl_renderer.h"

extern bool sresdlrenderer_tex_create(const sre_videodriver* video, void* p_texture, int w, int h, SDL_PixelFormatEnum format)
{
    SDL_Texture** p_sdltex = p_texture;
    
    #if 0
        SDL_Surface* dummy_fmtsurfacep = SDL_CreateRGBSurfaceWithFormat(0, w, h, 0, format);
        if (!dummy_fmtsurfacep)
            return false;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(video->userdata, dummy_fmtsurfacep);
        SDL_FreeSurface(dummy_fmtsurfacep);
    #else
        SDL_Texture* texture = SDL_CreateTexture(video->userdata, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, w, h);
    #endif
    if (!texture)
        return false;

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    *p_sdltex = texture;
    return true;
}

extern bool sresdlrenderer_tex_update(const sre_videodriver* video, void* texture, const void* pixels, int pitch)
{
    SDL_Texture** sdltex = texture;
    if (!*sdltex) return false;

    return SDL_UpdateTexture(*sdltex, NULL, pixels, pitch) == 0;
}

extern void sresdlrenderer_tex_destroy(const sre_videodriver* video, void* texture)
{
    SDL_Texture** sdltex = texture;
    if (!*sdltex) return;

    SDL_DestroyTexture(*sdltex);
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
