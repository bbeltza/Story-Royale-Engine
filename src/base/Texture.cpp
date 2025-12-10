#include "../internal.h"
#include "../internal.hpp"

#include "Base/Texture.hpp"

Texture::Texture(const sre::Image& from_image): texture(SDL_CreateTextureFromSurface(engine.sdl_rendererhndl, static_cast<SDL_Surface*>(from_image.getHandle())))
{
}

Texture::~Texture()
{
    if (texture)
    {
        if (!SDL_WasInit(SDL_INIT_VIDEO))
        {
            WARN("Destroying texture after SDL's deinitialization or before its initialization (Note that it shouldn't ever be possible when I rewrite the entire engine's architecture :) )");
            return;
        }

        SDL_Texture* tmp = static_cast<SDL_Texture*>(texture);
        texture = NULL;
        SDL_DestroyTexture(tmp);
    }
}

Vector2i Texture::size() const
{
    if (!texture) return Vector2i::ZERO;

    int w, h;
    SDL_QueryTexture(static_cast<SDL_Texture*>(texture), NULL, NULL, &w, &h);

    return { w, h };
}