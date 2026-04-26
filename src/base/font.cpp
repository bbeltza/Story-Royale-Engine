#include <Base/Font.hpp>
#include <Base/File.hpp>
#include <Base/Image.hpp>
#include <Base/Error.h>

using namespace sre;

Font::Font(const char* path, int pt): Font(sre::File(path), pt)
{
}

Font::Font(const sre::File& file, int pt)
{
    if (!file.valid())
        return;

    m_rwops = file.to_RWops();
    m_font = TTF_OpenFontRW(m_rwops, 1, pt);

    if (!m_font)
    {
        sre::error(SRE_ERR_CREATE, "Font::Font() could not load font");
        sre::error(SRE_ERR_SDL);
        if (m_rwops)
        {
            SDL_RWclose(m_rwops);
            m_rwops = NULL;
            return;
        }
    }

    // Setup ascii textures
        int wsum = 0;
        int hmax = 0;
        std::array<sre::Image, 127> images;
        for (size_t code = 0; code < images.size(); code++)
        {
            char str[2] = { static_cast<char>(code + 1), '\0' };
            auto& img = images.at(code);
            img = TTF_RenderUTF8_Solid(m_font, str, sre::WHITE.toSDL());

            sre::vec2i size = img.size();
            wsum += size.x;
            hmax = ut_max(hmax, size.y);
        }

        constexpr int BASE_MULTIPLE = 64;
        int dim = BASE_MULTIPLE;
        while ((wsum/dim) > (dim/hmax))
            dim += BASE_MULTIPLE;
        assert(hmax < dim);
        sre::Image ascii_imgatlas{dim};

        sre::vec2i pos;
        for (size_t i = 0; i < images.size(); i++)
        {
            auto &img = images.at(i);
            sre::vec2i size = img.size();
            if (pos.x + size.x > dim)
            {
                pos.x = 0;
                pos.y += hmax;
            }

            ascii_imgatlas.blit(img, pos, 0);
            auto &uv = ascii_uvs.at(i);
            uv = {
                pos / static_cast<float>(dim),
                size / static_cast<float>(dim)
            };

            pos.x += size.x;
        }

        ascii_atlas = ascii_imgatlas.to_sampler();
    //
}

Font::~Font()
{
    if (m_font && TTF_WasInit())
        TTF_CloseFont(m_font);
    else if (m_rwops)
        SDL_RWclose(m_rwops);
}

int Font::str_to_utf8chr(const char* str, char* dst)
{
    *reinterpret_cast<int*>(dst) = 0;
    if (!str[0]) return 0;

    if (str[0] > 0)
    {
        dst[0] = str[0];
        return 1;
    }
    else if (str[0] < static_cast<char>(-64)) assert(0 && "Broken UTF-8 codepoint");
    else if (str[0] < static_cast<char>(-32))
    {
        dst[0] = str[0];
        dst[1] = str[1];
        return 2;
    }
    else if (str[0] < static_cast<char>(-16))
    {
        dst[0] = str[0];
        dst[1] = str[1];
        dst[2] = str[2];
        return 3;
    }
    else
    {
        dst[0] = str[0];
        dst[1] = str[1];
        dst[2] = str[2];
        dst[3] = str[3];
        dst[4] = '\0';
        return 4;
    }

    return -1;
}