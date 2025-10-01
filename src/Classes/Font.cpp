#include "Datatypes/Color.hpp"
#include "Classes/Font.hpp"

#include "Engine.hpp"

Font::Font(const char* path, int pt)
{
    m_file.Load(path);
    m_font = TTF_OpenFontRW(SDL_RWFromConstMem(m_file.getRawData(), m_file.getSize()), 1, pt);
}

Font::~Font()
{
    if (TTF_WasInit()) TTF_CloseFont(m_font);
}

bool Font::PreloadTextures(const char* text)
{
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