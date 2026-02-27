#include <Base/Font.hpp>
#include <Base/File.hpp>

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
        sre::log<LOGCATEGORY_ERROR>("Font::Font() could not load font: %s", TTF_GetError());
        if (m_rwops)
            goto FAIL_CLOSE;
    }

    // Setup ascii textures
        for (size_t code = 0; code < ascii.size(); code++)
        {
            char str[2] = { static_cast<char>(code + 1), '\0' };

            auto& tex = ascii.at(code);
            tex = sre::Image{TTF_RenderUTF8_Solid(m_font, str, sre::col4::WHITE.toSDL())};
        }
    //
    return;

    FAIL_CLOSE:
    SDL_RWclose(m_rwops);
    m_rwops = NULL;
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
    else if (str[0] < -64) assert(0 && "Broken UTF-8 codepoint");
    else if (str[0] < -32)
    {
        dst[0] = str[0];
        dst[1] = str[1];
        return 2;
    }
    else if (str[0] < -16)
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