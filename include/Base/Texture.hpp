#pragma once
#include <datatypes/vector.hpp>
#include <datatypes/color.hpp>
#include <datatypes/rect.hpp>
#include <Base/Image.hpp>

#include <Base/Texture.h>

namespace sre
{
    using TextureId = sre_Texture;

    #define Texture _Texture
    class Texture
    {
        Texture(const Texture& other) = delete;
        
    protected:
        TextureId m_handle = 0;
    public:
        Texture() = default;
        Texture(const Image& from_image): m_handle( sre_tex_gen() )
        {
            sre_tex_bind(
                m_handle,
                static_cast<SDL_Surface*>(from_image.getHandle()),
                true
            );
        }

        Texture(Texture&& moving): m_handle(moving.m_handle) { moving.m_handle = 0; }
        ~Texture() { sre_tex_destroy(m_handle); }

        sre::vec2i size() const
        {
            int _size[2];
            sre_tex_size(m_handle, _size + 0, _size + 1);
            return { _size[0], _size[1] };
        }
    };
    #undef Texture
}

class Texture
{
    friend struct _containers_service;

    Texture(const Texture& other) = delete;
public:
    Texture() = default;
    Texture(const sre::Image& from_image);

    Texture(Texture&& moving) noexcept;
    ~Texture();

    sre::vec2i size() const;

    void* handle() const { return texture; }

protected: // Protected to allow custom behavior inherited texture classes
    void* texture = NULL;
};