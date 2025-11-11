#include "Datatypes/Color.hpp"
#include "Base/Font.hpp"

#include "Engine.hpp"

#include "../internal.h"

Font::Font(const char* path, int pt)
{
    File file(path);
    m_font = TTF_OpenFontRW(file.toRWops(), 1, pt);

    _fonts_loaded->push_front(m_font);
}

Font::~Font()
{
    if (!TTF_WasInit()) return;

    _fonts_loaded->remove(m_font);
    TTF_CloseFont(m_font);
}

bool Font::PreloadTextures(const char* text)
{
    if (!text[0]) return true;

    bool unloaded = false;
    size_t n = 0;
    while (text[n])
    {
        char c = text[n];
        if (!textures.count(c)) {textures.emplace(c, TTF_RenderGlyph_Solid(m_font, c, Color4::WHITE)); unloaded = true;};
        n++;
    }

    return unloaded;
}