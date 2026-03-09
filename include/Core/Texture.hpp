#ifndef SRE_TEXTURE_HPP
#define SRE_TEXTURE_HPP
#include <Base/Image.hpp>
#include <Core/Texture.h>

#include <Core/Error.h>
#include <Base/Log.h>

namespace sre
{
    using TextureId = sre_Texture;

    class Texture
    {
        Texture(const Texture& other) = delete;

    protected:
        TextureId m_handle = 0;
    public:
        Texture() = default;
        Texture(const sre::vec2i size, SDL_PixelFormatEnum format): m_handle(sre_tex_create(size.x, size.y, format)) {}
        Texture(const Image& from_image):
            Texture(from_image.size(), from_image.SDLformat())
        {
            SDL_PixelFormatEnum format_enum = sre_tex_format(m_handle);

            SDL_Surface* new_surfacetmp = SDL_ConvertSurfaceFormat(
                static_cast<SDL_Surface*>(from_image.handle()),
                format_enum,
                0
            );
            if (!new_surfacetmp)
            {
                sre::oerr << "Failed creating texture from image: " << SDL_GetError() << sre::endl; 
                return;
            }

            if (!sre_tex_update(m_handle, new_surfacetmp->pixels, new_surfacetmp->pitch))
                sre::oerr << "Failed creating texture from image: sre_tex_update() fail" << sre::endl;
            
            SDL_FreeSurface(new_surfacetmp);
        }

        Texture(const Image* from_imagep): Texture(*from_imagep) {}

        Texture(Texture&& moving) noexcept: m_handle(moving.m_handle) { moving.m_handle = 0; }
        ~Texture() { sre_tex_destroy(m_handle); }

        bool update(const void* pixels, int pitch) const { return sre_tex_update(m_handle, pixels, pitch); }
        
        void operator =(Texture&& moving) noexcept { sre_tex_destroy(m_handle); m_handle = moving.m_handle; moving.m_handle = 0; }

        sre::vec2i size() const
        {
            int _size[2];
            sre_tex_size(m_handle, _size + 0, _size + 1);
            return { _size[0], _size[1] };
        }

        TextureId handle() const { return m_handle; }

    };
}

#endif