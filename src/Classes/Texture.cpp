#include "Engine.h"
#include "Classes/Texture.h"

Texture::Texture(Texture&& moving): 
    m_file(std::move(moving.m_file)),
    texture(moving.texture),
    file_surface(moving.file_surface)
{
    moving.texture = nullptr;
    moving.file_surface = nullptr;
}

Texture::Texture(const char* path)
{
    m_file.Load(path);

    {
        SDL_RWops* rw = SDL_RWFromConstMem(m_file.getRawData(), m_file.getSize());
        file_surface = IMG_Load_RW(rw, 1);
        Engine->DrawingContext.textures_toload.push_back(this);
    }
}

Texture::Texture(int w, int h): texture(SDL_CreateTexture(Engine->DrawingContext.sdl_renderer, 0, SDL_TEXTUREACCESS_STREAMING, w, h))
{
    SDL_SetTextureBlendMode(m_Texture, SDL_BLENDMODE_BLEND);
}

Texture::~Texture()
{
    if (file_surface) SDL_FreeSurface(m_Surface);
    if (texture) SDL_DestroyTexture(m_Texture);
}

Vector2i Texture::GetSize()
{
    if (!texture)
        return {m_Surface->w, m_Surface->h};

    int w, h;
    SDL_QueryTexture(m_Texture, NULL, NULL, &w, &h);
    return {w, h};
}