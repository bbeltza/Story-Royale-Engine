#include "../internal.h"

#include "Engine.hpp"
#include "Base/Texture.hpp"

Texture::Texture(Texture&& moving) noexcept: 
    texture(moving.texture),
    file_surface(moving.file_surface)
{
    moving.texture = nullptr;
    moving.file_surface = nullptr;
    for (auto& el : *to_load)
    {
        if (el == &moving)
        {
            el = this;
            return;
        }
    }
}

Texture::Queue* Texture::to_load = nullptr;

Texture::Texture(const char* path)
{
    File file;
    file.Load(path);

    {
        SDL_RWops* rw = SDL_RWFromConstMem(file.getRawData(), (int)file.getSize());
        file_surface = IMG_Load_RW(rw, 1);
    }
    push_queue();
}

Texture::Texture(void* from_surface): file_surface(from_surface)
{
    push_queue();
}

Texture::~Texture()
{
    if (!SDL_WasInit(0)) return;

    if (file_surface) SDL_FreeSurface(m_Surface);
    if (texture) SDL_DestroyTexture(m_Texture);
    file_surface = nullptr;
    texture = nullptr;
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
    static Queue queue;
    to_load = &queue;
    queue.push_back(this);
}