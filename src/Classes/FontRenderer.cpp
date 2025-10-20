#include "Classes/Font.hpp"

#include "Engine.hpp"

static std::vector<std::string> line_buff;
//static std::vector<int> width_buff;

void Font::Render(const RectF & bounds, const char* text, int count, HAlign halignment, VAlign valignment)
{
    if (PreloadTextures(text)) return;
    if (count == 0) return;

    float line_skip = (float)TTF_FontLineSkip(m_font);

    const int width = (int)bounds.Size.X;
    int last_begin, i = 0, done = 0;

    while (true)
    {
        if (line_buff.size() <= (size_t)i) line_buff.resize(i+1);

        TTF_MeasureText(m_font, text, width, NULL, &last_begin);
        int end = last_begin;
        for (int j = 0; j < end; j++)
        {
            char chr = text[j];
            if (chr == '\n')
            {
                last_begin = j+1;
                break;
            }
            else if (chr == ' ')
                last_begin = j+1;
            else if (!text[j+1]) {
                last_begin = end;
                done = 1;
            }
        }
        int diff = 0;
        if (text[last_begin - 1] == '\n') diff = 1;

        line_buff[i].assign(text, last_begin - diff);
        i++;
        if (done) break;

        text += last_begin;
    }

    Vector2f startvec = bounds.Position;

    float yoffset = 0;
    if (valignment != VTop) yoffset = bounds.Size.Y - line_skip * i;
    if (valignment == VCenter) yoffset /= 2;

    startvec.Y += yoffset;

    size_t n = 0;

    for (int j = 0; j < i; j++)
    {
        const char* c_str = line_buff[j].c_str();

        int line_extent;
        TTF_MeasureText(m_font, c_str, width, &line_extent, NULL);

        float xoffset = 0;
        if (halignment != HLeft) xoffset = bounds.Size.X - line_extent;
        if (halignment == HCenter) xoffset /= 2;

        startvec.X = bounds.Position.X + xoffset;

        RenderLine(startvec, c_str, count, (int)n);

        startvec.Y += line_skip;
        n += strlen(c_str);
    }
}

void Font::RenderLine(const Vector2f& start, const char* text, int count, int acc)
{
    if (count > 0 && count - acc <= 0) return;

    count = count < 0 ? -1 : count;
    size_t scount = (size_t)count, n = 0;
    scount -= acc;

    char chr = text[n];

    RectF render_rect(start, Vector2f::ZERO);
    while (chr)
    {
        if (n >= scount) break;

        Texture& texture = textures.at(chr);
        render_rect.Size = texture.GetSize();

        Display::DrawTexture(texture, render_rect, Color4::WHITE, Vector2f::ZERO);

        render_rect.Position.X += render_rect.Size.X;

        chr = text[++n];
    }
}