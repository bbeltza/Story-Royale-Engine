#include "../internal.h"
#include "../internal.hpp"

#include "Engine.hpp"
#include "Base/Texture.hpp"

Texture::Texture(Texture&& moving) noexcept: 
    texture(moving.texture),
    file_surface(moving.file_surface)
{
    _containers->lock();
    _containers->loaded_textures.remove(&moving);
    if (texture) _containers->loaded_textures.push_front(this);
    _containers->unlock();
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
    File file(path);
    SDL_RWops* rw = file.toRWops();
    file_surface = IMG_Load_RW(rw, 1);

    push_queue();
}

Texture::Texture(void* from_surface): file_surface(from_surface)
{
    push_queue();
}

Texture::~Texture()
{
    if (engine.containers_service)
    {
        _containers->lock();
        _containers->loaded_textures.remove(this);
        _containers->unlock();
    }
    if (file_surface) SDL_FreeSurface(m_Surface);
    if (texture) SDL_DestroyTexture(m_Texture);
    file_surface = nullptr;
    texture = nullptr;
}

Vector2i Texture::GetSize()
{
    if (!texture)
    {
        if (file_surface)
            return {m_Surface->w, m_Surface->h};
        else
            return {0, 0};
    }

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