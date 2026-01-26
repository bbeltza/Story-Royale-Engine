#ifndef SRE_TEXTURE_HPP
#define SRE_TEXTURE_HPP
#include <Base/Image.hpp>
#include <Core/Texture.h>

namespace sre
{
    extern "C" void ERROR(const char* fmt, ...);

    using TextureId = sre_Texture;

    class Texture
    {
        Texture(const Texture& other) = delete;

    protected:
        TextureId m_handle = 0;
    public:
        Texture() = default;
        Texture(const Image& from_image) : m_handle(sre_tex_gen()) { create(from_image); }

        Texture(Texture&& moving) noexcept: m_handle(moving.m_handle) { moving.m_handle = 0; }
        ~Texture() { sre_tex_destroy(m_handle); }

        void operator =(Texture&& moving) noexcept { sre_tex_destroy(m_handle); m_handle = moving.m_handle; moving.m_handle = 0; }
        void operator =(const Image* from_image) { operator =(*from_image); }
        void operator =(const Image& from_image) { create(from_image); }

        sre::vec2i size() const
        {
            int _size[2];
            sre_tex_size(m_handle, _size + 0, _size + 1);
            return { _size[0], _size[1] };
        }

        TextureId handle() const { return m_handle; }

    private:
        void create(const Image& from_image)
        {
            if (!m_handle)
            {
                ERROR("Texture::Texture(): Failed to create texture on sre_tex_gen()");
                return;
            }
            if (!from_image.valid())
                return;

            if (sre_tex_bind(
                m_handle,
                static_cast<SDL_Surface*>(from_image.handle())
            ) < 0) ERROR("Texture::Texture(): Failed to create texture on sre_tex_bind()");
        }
    };
}

#endif