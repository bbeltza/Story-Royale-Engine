#include "../internal.h"
#include "../internal.hpp"
#include "../core/drivers/drivers.h" // TODO: Add driver speficic texture creation

#include "Base/Texture.hpp"

Texture::Texture(const sre::Image& from_image): texture(SDL_CreateTextureFromSurface(static_cast<SDL_Renderer*>(engine.video->userdata), static_cast<SDL_Surface*>(from_image.getHandle())))
                                                                                                                            // ^^^
                                                                                                                            // This is now slightly better
                                                                                                                            // Still need to make a custom texture handler
                                                                                                                            // Because userdata isn't going to be a "SDL_Renderer" in every driver
{
}

Texture::~Texture()
{
    if (texture)
    {
        if (!SDL_WasInit(SDL_INIT_VIDEO))
        {
            static bool warned;
            if (!warned)
            {
                WARN("Destroying texture after SDL's deinitialization or before its initialization (Note that it shouldn't ever be possible when I rewrite the entire engine's architecture :) )");
                warned = true;
            }
            return;
        }

        SDL_Texture* tmp = static_cast<SDL_Texture*>(texture);
        texture = NULL;
        SDL_DestroyTexture(tmp);
    }
}

sre::vec2i Texture::size() const
{
    if (!texture) return sre::vec2i::ZERO;

    int w, h;
    SDL_QueryTexture(static_cast<SDL_Texture*>(texture), NULL, NULL, &w, &h);

    return { w, h };
}