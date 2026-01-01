#include "Base/Font.hpp"

#include "Engine.hpp"

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
        char c = text[n];
        if (!textures.count(c))
        {
            textures.emplace(
                c,
                sre::Image{TTF_RenderGlyph_Solid(m_font, c, sre::col4::WHITE.toSDL())});
            unloaded = true;
        };
        n++;
    }

    return unloaded;
}