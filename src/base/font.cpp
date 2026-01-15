#include <Base/Font.hpp>
#include <Base/File.hpp>

using namespace sre;

Font::Font(const char *path, int pt)
{
    sre::File file(path);
    if (!file.valid())
        return;

    m_rwops = file.to_RWops();
    m_font = TTF_OpenFontRW(m_rwops, 1, pt);
    if (!m_font)
    {
        ERROR("Font::Font() could not load font from '%s': %s", path, TTF_GetError());
        if (m_rwops)
        {
            SDL_RWclose(m_rwops);
            m_rwops = NULL;
        }
    }
}

Font::~Font()
{
    if (m_font && TTF_WasInit())
        TTF_CloseFont(m_font);
    else if (m_rwops)
        SDL_RWclose(m_rwops);
}

bool Font::preload(const char *text)
{
    if (!m_font)
        return true;
    if (!text[0])
        return true;

    bool unloaded = false;
    size_t n = 0;
    while (text[n])
    {
        char utf8[8] = { 0 };
        n += str_to_utf8chr(text + n, utf8);

        if (!textures.count(*reinterpret_cast<int*>(utf8)))
        {
            
            textures.emplace(
                *reinterpret_cast<int*>(utf8),
                sre::Image{TTF_RenderUTF8_Solid(m_font, utf8, sre::col4::WHITE.toSDL())});
            unloaded = true;
        };
    }

    return unloaded;
}

int Font::str_to_utf8chr(const char* str, char* dst)
{
    register int inc = 0;
    if (!str[0]) goto END;

    if (str[0] > 0)
    {
        dst[0] = str[0];
        inc = 1;
    }
    else if (str[0] < -64) assert(0 && "Broken UTF-8 codepoint");
    else if (str[0] < -32)
    {
        dst[0] = str[0];
        dst[1] = str[1];
        inc = 2;
    }
    else if (str[0] >= -16)
    {
        dst[0] = str[0];
        dst[1] = str[1];
        dst[2] = str[2];
        inc = 3;
    }
    else
    {
        dst[0] = str[0];
        dst[1] = str[1];
        dst[2] = str[2];
        dst[3] = str[3];
        inc = 4;
    }    
    
    assert(inc && "Broken UTF-8 Codepoint");

    END:
    dst[inc] = '\0';
    return inc;
}