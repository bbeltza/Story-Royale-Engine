#include "datatypes/Color.hpp"
#include "Base/Font.hpp"

#include "Engine.hpp"

#include "../internal.h"
#include "../internal.hpp"

Font::Font(const char* path, int pt)
{
    _containers->lock();

    File file(path);
    m_font = TTF_OpenFontRW(file.toRWops(), 1, pt);
    
    _containers->loaded_fonts.push_front(this);

    _containers->unlock();
}

Font::~Font()
{
    if (!engine.containers_service) return;

    _containers->lock();
    if (m_font)
        TTF_CloseFont(m_font);
    
    _containers->loaded_fonts.remove(this);

    _containers->unlock();
}

bool Font::PreloadTextures(const char* text)
{
    if (!text[0]) return true;

    bool unloaded = false;
    size_t n = 0;
    while (text[n])
    {
        char c = text[n];
        if (!textures.count(c)) 
        {
            textures.emplace(
                c,
                sre::Image{ TTF_RenderGlyph_Solid(m_font, c, Color4::WHITE) }
            );
            unloaded = true;
        };
        n++;
    }

    return unloaded;
}