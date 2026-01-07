#include "../internal.h"
#include "../internal.hpp"
#include "../core/drivers/drivers.h" // TODO: Add driver speficic texture creation

#include "Base/Texture.hpp"

Texture::Texture(const sre::Image& from_image): texture(SDL_CreateTextureFromSurface(*reinterpret_cast<SDL_Renderer**>(engine.video + 1), static_cast<SDL_Surface*>(from_image.getHandle())))
                                                                                                                                // ^^^
                                                                                                                                // This is the very silly way to take once for all the renderer
                                                                                                                                // Since I know it is located there
                                                                                                                                // But once I start to add multiple video drivers like OpenGL...
                                                                                                                                // It's not going to work out, a custom texture handler is needed
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