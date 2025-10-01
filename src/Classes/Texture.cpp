#include "Engine.hpp"
#include "Classes/Texture.hpp"

Texture::Texture(Texture&& moving): 
    texture(moving.texture),
    file_surface(moving.file_surface)
{
    moving.texture = nullptr;
    moving.file_surface = nullptr;
}

Texture::Texture(const char* path)
{
    File file;
    file.Load(path);

    {
        SDL_RWops* rw = SDL_RWFromConstMem(file.getRawData(), file.getSize());
        file_surface = IMG_Load_RW(rw, 1);
    }
    push_queue();
}

Texture::Texture(void* from_surface): file_surface(from_surface)
{
    push_queue();
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

void Texture::push_queue()
{
    Engine->DrawingContext.textures_toload.push_back(this);
}