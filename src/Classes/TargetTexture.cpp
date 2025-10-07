#include "Engine.hpp"
#include "Classes/TargetTexture.hpp"

#define setup_aliases   auto& target_textures = Engine->DrawingContext.target_textures; \
                        auto& targetptr = Engine->DrawingContext.targetptr; \
                        auto renderer = Engine->DrawingContext.sdl_renderer;

#define m_texture reinterpret_cast<SDL_Texture*>(m_handle)
TargetTexture::TargetTexture(): TargetTexture({Engine->DrawingContext.m_viewport.w, Engine->DrawingContext.m_viewport.h})
{
    has_customsize = false;
}

TargetTexture::TargetTexture(const Vector2i& size): has_customsize(true)
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
                size.X,
                size.Y
            ));
    
    m_handle = target_textures.at(targetptr);

    SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, m_texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
}

TargetTexture::~TargetTexture()
{
    setup_aliases

    if (has_customsize)
    {
        SDL_DestroyTexture(m_texture);
        target_textures.pop_back();
    }

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

Texture TargetTexture::CreateTexture(const RectI& src)
{
    setup_aliases

    const SDL_Rect* rect = reinterpret_cast<const SDL_Rect*>(&src);
    int w = rect->w, h = rect->h;
    if (src.Size.X == 0 || src.Size.Y == 0)
    {
        rect = NULL;
        SDL_QueryTexture(m_texture, NULL, NULL, &w, &h);
    }
    syslogln("%d %d", w, h);

    uint64_t* pixeldata = new uint64_t[w * h];

    SDL_SetRenderTarget(renderer, m_texture);
    SDL_RenderReadPixels(renderer, rect, 0, pixeldata, w*4);
    SDL_SetRenderTarget(renderer, target_textures.at(targetptr));

    SDL_Texture* sdl_texture = SDL_CreateTexture(renderer, NULL, SDL_TEXTUREACCESS_STATIC, w, h);
    SDL_SetTextureBlendMode(sdl_texture, SDL_BLENDMODE_BLEND);
    SDL_UpdateTexture(sdl_texture, NULL, pixeldata, w*4);

    Texture texture;
    texture.texture = sdl_texture;

    return texture;
}