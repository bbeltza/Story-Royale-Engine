#include <Core/Render.h>
#include <Base/Font.hpp>
#include <Base/Image.hpp>

using namespace sre;

void Font::render(const sre::rect2Dut &bounds, const sre::col4 &color, const char *text, int count, Alignment halignment, Alignment valignment)
{
    if (text[0] == '\0')
        return;
    if (count == 0)
        return;
    if (bounds.size.x <= 0)
        return;

    sre::unit line_skip = (sre::unit)TTF_FontLineSkip(m_font);

    const int width = (int)bounds.size.x;
    int last_begin, i = 0, done = 0;

    while (true)
    {
        if (linebuf.size() <= (size_t)i)
            linebuf.resize(i + 1);

        TTF_MeasureText(m_font, text, width, NULL, &last_begin);
        int end = last_begin;
        assert(end != 0);
        
        for (int j = 0; j < end; j++)
        {
            char chr = text[j];
            if (chr == '\n')
            {
                last_begin = j + 1;
                break;
            }
            else if (chr == ' ')
                last_begin = j + 1;
            else if (!text[j + 1])
            {
                last_begin = end;
                done = 1;
            }
        }
        int diff = 0;
        if (text[last_begin - 1] == '\n')
            diff = 1;

        linebuf[i].assign(text, last_begin - diff);
        i++;
        if (done)
            break;

        text += last_begin;
    }

    sre::vec2ut startvec = bounds.position;

    sre::unit yoffset = 0;
    if (valignment != A_TOP)
        yoffset = bounds.size.y - line_skip * i;
    if (valignment == A_CENTER)
        yoffset /= 2;

    startvec.y += yoffset;

    size_t n = 0;

    for (int j = 0; j < i; j++)
    {
        const char *c_str = linebuf[j].c_str();

        int line_extent;
        TTF_MeasureText(m_font, c_str, width, &line_extent, NULL);

        sre::unit xoffset = 0;
        if (halignment != A_LEFT)
            xoffset = bounds.size.x - line_extent;
        if (halignment == A_CENTER)
            xoffset /= 2;

        startvec.x = bounds.position.x + xoffset;

        render_line(startvec, color, c_str, count, (int)n);

        startvec.y += line_skip;
        n += strlen(c_str);
    }
}

void Font::render_line(const sre::vec2ut &start, const sre::col4 &color, const char *text, int count, int acc)
{
    assert(m_font);
    if (count > 0 && count - acc <= 0)
        return;

    count = count < 0 ? -1 : count;
    size_t scount = (size_t)count;
    size_t n = 0;
    scount -= acc;

    sre::rect2Dut render_rect(start, sre::vec2ut::ZERO);
    while (text[n])
    {
        if (n >= scount)
            break;
        
        Sampler* texture;

        if (text[n] > 0)
        {
            texture = ascii.at(text[n] - 1).get();
            n++;
        }
        else
        {
            char utf8[8];
            int codepoint;
            n += str_to_utf8chr(text + n, utf8);

            codepoint = *reinterpret_cast<int*>(utf8);
            if (unicode.find(codepoint) == unicode.end())
                unicode.emplace(codepoint, sre::Image{TTF_RenderUTF8_Solid(m_font, utf8, sre::WHITE.toSDL())}.to_sampler());
            
            texture = unicode.at(codepoint).get();
        }

        sre::vec2i tsize = texture->size();
        render_rect.size = sre::vec2ut{tsize};

        sre::render_draw1(
            0, {{
                render_rect,
                vec2ut::ZERO,
                color,
                0,
                { 1, 1 },
                { 0, 0 }
            }}, texture
        );

        render_rect.position.x += render_rect.size.x;
    }
}