#include "Datatypes/Color.h"
#include "Classes/Font.h"

#include "Engine.h"

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

void Font::Render(const RectF& bounds, const char* text, int count, HAlign halignment, VAlign valignment)
{
    if (PreloadTextures(text)) return;
    if (count == 0) return;

    int extent, c;
    TTF_MeasureTextSpaced(m_font, text, bounds.Size.X, &extent, &c);

    syslogln("%d %d", extent, c);

    size_t scount = count < 0 ? (size_t)-1 : count;
    size_t n = 0;
    RectF advance = bounds;
    advance.Position.Y -= 3;
    while (text[n])
    {
        if (n >= scount) break;
        char chr = text[n];
        
        Texture& texture = textures.at(chr);
        advance.Size = texture.GetSize();

        Engine->DrawingContext.DrawTexture(texture, advance, Color4::WHITE, Vector2f::ZERO);

        advance.Position.X += advance.Size.X;

        n++;
    }
}