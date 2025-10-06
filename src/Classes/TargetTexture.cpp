#include "Engine.hpp"
#include "Classes/TargetTexture.hpp"

#define setup_aliases   auto& target_textures = Engine->DrawingContext.target_textures; \
                        auto& targetptr = Engine->DrawingContext.targetptr; \
                        auto renderer = Engine->DrawingContext.sdl_renderer;

#define m_texture reinterpret_cast<SDL_Texture*>(m_handle)
TargetTexture::TargetTexture()
{
    assert(Engine && "Target texture must NOT be created statically and shouldn't be created before initializing");

    setup_aliases
    targetptr++;

    if (target_textures.size() <= targetptr)
        target_textures.push_back(
            SDL_CreateTexture(
                renderer, // DrawingContext's renderer (they are friends so it can access it heh)
                NULL, // Shouldn't care about the format on a rendering target
                SDL_TEXTUREACCESS_TARGET,
                Engine->DrawingContext.m_viewport.w,
                Engine->DrawingContext.m_viewport.h
            ));
    
    m_handle = target_textures.at(targetptr);

    SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, m_texture);
    SDL_RenderClear(renderer);
}

TargetTexture::~TargetTexture()
{
    setup_aliases
    targetptr--;

    SDL_SetRenderTarget(renderer, target_textures.at(targetptr));
}

void TargetTexture::Blit(const RectI& src, const RectF& dest, const Vector2f& anchor)
{
    setup_aliases
    const SDL_Rect* srcRect = reinterpret_cast<const SDL_Rect*>(&src); // They'll be the same
    const SDL_FRect destRect = {
        dest.Position.X - dest.Size.X * anchor.X,
        dest.Position.Y - dest.Size.Y * anchor.Y,
        dest.Size.X,
        dest.Size.Y
    };

    SDL_SetRenderTarget(renderer, target_textures.at(targetptr - 1));
    SDL_RenderCopyF(renderer, m_texture, srcRect, &destRect);
    SDL_SetRenderTarget(renderer, m_texture);
}