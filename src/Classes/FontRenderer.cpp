#include "Classes/Font.h"

#include "Engine.h"

static std::vector<std::string> line_buff;
static std::vector<int> width_buff;

void Font::Render(const RectF & bounds, const char* text, int count, HAlign halignment, VAlign valignment)
{
    if (PreloadTextures(text)) return;
    if (count == 0) return;

    const int width = bounds.Size.X;
    int last_begin, i = 0;

    while (true)
    {
        if (line_buff.size() <= i) line_buff.resize(i+1);
        TTF_MeasureText(m_font, text, width, NULL, &last_begin);
        for (int j = 0, end = last_begin; j < end; j++)
        {
            char chr = text[j];
            if (chr == '\n')
            {
                last_begin = j+1;
                break;
            }
            else if (chr == ' ')
                last_begin = j+1;
            else if (!chr) {
                line_buff[i].assign(text);
                i = -1;
            }
        }
        if (i < 0) break;

        line_buff[i].assign(text);
        syslogln("%d %s", i, line_buff[i].c_str());
        i++;
        text += last_begin + 1;
    }
}

void Font::RenderLine(const Vector2f& start, const char* text, int count)
{
    count = count < 0 ? -1 : count;
    size_t scount = (size_t)count, n = 0;

    char chr = text[n];

    RectF render_rect(start, Vector2f::ZERO);
    while (chr)
    {
        if (n >= scount) break;

        Texture& texture = textures.at(chr);
        render_rect.Size = texture.GetSize();

        Engine->DrawingContext.DrawTexture(texture, render_rect, Color4::WHITE, Vector2f::ZERO);

        render_rect.Position.X += render_rect.Size.X;

        end:
        chr = text[++n];
    }
}