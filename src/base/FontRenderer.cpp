#include "Base/Font.hpp"

#include "Engine.hpp"

static std::vector<std::string> line_buff;
//static std::vector<int> width_buff;

void Font::Render(const sre::rect2Dut & bounds, const char* text, int count, HAlign halignment, VAlign valignment)
{
    if (PreloadTextures(text)) return;
    if (count == 0) return;
    if (bounds.size.x <= 0) return;

    sre::unit line_skip = (sre::unit)TTF_FontLineSkip(m_font);

    const int width = (int)bounds.size.x;
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

    sre::vec2ut startvec = bounds.position;

    sre::unit yoffset = 0;
    if (valignment != VTop) yoffset = bounds.size.y - line_skip * i;
    if (valignment == VCenter) yoffset /= 2;

    startvec.y += yoffset;

    size_t n = 0;

    for (int j = 0; j < i; j++)
    {
        const char* c_str = line_buff[j].c_str();

        int line_extent;
        TTF_MeasureText(m_font, c_str, width, &line_extent, NULL);

        sre::unit xoffset = 0;
        if (halignment != HLeft) xoffset = bounds.size.x - line_extent;
        if (halignment == HCenter) xoffset /= 2;

        startvec.x = bounds.position.x + xoffset;

        RenderLine(startvec, c_str, count, (int)n);

        startvec.y += line_skip;
        n += strlen(c_str);
    }
}

void Font::RenderLine(const sre::vec2ut& start, const char* text, int count, int acc)
{
    if (count > 0 && count - acc <= 0) return;

    count = count < 0 ? -1 : count;
    size_t scount = (size_t)count, n = 0;
    scount -= acc;

    char chr = text[n];

    sre::rect2Dut render_rect(start, sre::vec2ut::ZERO);
    while (chr)
    {
        if (n >= scount) break;

        Texture& texture = textures.at(chr);
        render_rect.size = sre::vec2ut{texture.size()};

        Display::DrawTexture(texture, render_rect, sre::col4::WHITE, sre::vec2f::ZERO, DISPLAY_DONT_CENTER);

        render_rect.position.x += render_rect.size.x;

        chr = text[++n];
    }
}