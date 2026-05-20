#include <Base/Font.hpp>
#include <Base/Error.h>
#include <Base/Log.h>

using namespace sre;

FontAtlas::FontAtlas(const sre::RSampler& texture, sre::vec2i contant_size): m_atlas(texture), m_lineskip(static_cast<sre::unit>(contant_size.y))
{
    if (!m_atlas)
        return;
    
    sre::vec2f texsizef{texture->size()};
    sre::vec2f csizef{contant_size};

    sre::vec2f csizeuv = csizef / texsizef;
    sre::vec2f positionuv{0};

    if (csizeuv.x > 1)
        sre::critical(SRE_ERR_CREATE, "Failed to build font atlas when `constant_size.x` overflows `texture`'s dimensions");

    for (int i = 0; i < CHARACTER_COUNT; i++)
    {
        if (positionuv.x + csizeuv.x > 1)
        {
            positionuv.x = 0;
            positionuv.y += csizeuv.y;

            if (positionuv.y + csizeuv.y > 1)
                sre::log(SRE_LOG_WARN "The texture atlas is too small to fit in every single character, although it might work. Some characters' vertical uv region will surpass `1` so they might not be rendered at all.");
        }
        m_glyphset[i].uvregion = { positionuv, csizeuv };
        m_glyphset[i].advance = csizef.x;
        m_glyphset[i].bearing = {0};

        positionuv.x += csizeuv.x;
    }
}

FontAtlas::FontAtlas(const sre::RSampler& texture, const sre::Glyph glyphs[CHARACTER_COUNT], sre::unit lineskip, bool normalize): m_atlas(texture), m_lineskip(lineskip)
{
    if (!m_atlas)
        return;

    memcpy(m_glyphset, glyphs, sizeof(m_glyphset));
    if (normalize)
    {
        sre::vec2f texsizef{texture->size()};
        for (int i = 0; i < CHARACTER_COUNT; i++)
        {
            m_glyphset[i].uvregion.position /= texsizef;
            m_glyphset[i].uvregion.size /= texsizef;
        }
    }
}

//

void FontAtlas::render_text(const FontRenderData& renderdata, const FontRenderTextData& textdata, const UnicodeSet* unicodeset, void* userdata)
{
    struct LineData
    {
        LineData(const char* strptr): ptr(strptr) {}

        const char* ptr;
        unsigned num_chars = 0;
        sre::unit extent = 0;
    };

    thread_local std::vector<LineData> linebuf;
    const char* last_space = NULL;
    sre::unit lastextent_space = 0;

    linebuf.clear();
    linebuf.emplace_back(renderdata.text);

    unsigned textlen = static_cast<unsigned>(renderdata.textlen);
    for (unsigned i = 0; i < textlen && renderdata.text[i]; i++)
    {
        unsigned chr_index = renderdata.text[i];
        if (chr_index > 127)
        {
            //if (!unicodeset)
            {
                sre::log(SRE_LOG_WARN "`renderdata.text` contains a non-ascii sequence at index %d. Unicode is not yet supported", i);
                continue;
            }
        }

        if (chr_index < 32 && chr_index != '\n')
            continue;

        chr_index -= 32;

        const sre::Glyph& glyph = m_glyphset[chr_index];
        LineData* linedat = &linebuf.back();

        if (chr_index == 0)
        {
            last_space = renderdata.text + i;
            lastextent_space = linedat->extent + glyph.advance;
        }
        else if (chr_index == static_cast<unsigned>('\n' - 32))
        {
            last_space = NULL;
            linebuf.emplace_back(linedat->ptr + linedat->num_chars + 1);
            continue;
        }

        if (linedat->extent + glyph.advance > textdata.area.size.x)
        {
            int spacediff;
            sre::unit extentdiff;
            if (last_space)
            {
                extentdiff = linedat->extent - lastextent_space;
                spacediff = static_cast<int>(&renderdata.text[i] - last_space - 1);
                linedat->num_chars -= spacediff;
                linedat->extent = lastextent_space;
            }
            
            linebuf.emplace_back(linedat->ptr + linedat->num_chars);
            linedat = &linebuf.back();
            if (last_space)
            {
                linedat->num_chars += spacediff;
                linedat->extent += extentdiff;
                last_space = NULL;
            }
        }

        linedat->num_chars++;
        linedat->extent += glyph.advance;
    }

    unsigned charsleft = static_cast<unsigned>(textdata.char_count);
    FontRenderData renderlinedata{ renderdata };
    sre::vec2ut cursor = textdata.area.position;
    switch (textdata.v_alignment)
    {
        case sre::ALIGN_TOP: break;
        case sre::ALIGN_CENTER: cursor.y += (textdata.area.size.y - linebuf.size()*m_lineskip)/2; break;
        case sre::ALIGN_BOTTOM: cursor.y += textdata.area.size.y - linebuf.size()*m_lineskip; break;
        default: abort();
    }

    for (const LineData& linedat : linebuf)
    {
        switch (textdata.h_alignment)
        {
            case sre::ALIGN_LEFT: break;
            case sre::ALIGN_CENTER: cursor.x += (textdata.area.size.x - linedat.extent)/2; break;
            case sre::ALIGN_RIGHT: cursor.x += textdata.area.size.x - linedat.extent; break;
            default: abort();
        }

        renderlinedata.text = linedat.ptr;
        renderlinedata.textlen = ut_min(linedat.num_chars, charsleft);
        render_line(renderlinedata, cursor, unicodeset, userdata);

        if (charsleft < linedat.num_chars)
            break;
        charsleft -= linedat.num_chars;
        cursor.y += m_lineskip;
        cursor.x = textdata.area.position.x;
    }
}

void FontAtlas::render_line(const FontRenderData& renderdata, sre::vec2ut topleft_begin, const UnicodeSet* unicodeset, void* userdata)
{
    const sre::vec2f sizef_atlas{m_atlas->size()};

    unsigned textlen = static_cast<unsigned>(renderdata.textlen);
    for (unsigned i = 0; i < textlen && renderdata.text[i]; i++)
    {
        unsigned chr_index = renderdata.text[i];
        if (chr_index > 127)
        {
            //if (!unicodeset)
            {
                sre::log(SRE_LOG_WARN "`renderdata.text` contains a non-ascii sequence at index %d. Unicode is not yet supported", i);
                continue;
            }
        }

        if (chr_index < 32)
            continue;

        chr_index -= 32;

        const sre::Glyph& glyph = m_glyphset[chr_index];
        const sre::vec2ut chrsize = glyph.uvregion.size * sizef_atlas;
        sre::RenderInstance1 data[] = {
            {
                { topleft_begin + glyph.bearing, chrsize },
                { 0, 0 },
                renderdata.color,
                0,

                glyph.uvregion.size,
                glyph.uvregion.position
            }
        };

        if (renderdata.modifier_callback)
            renderdata.modifier_callback(userdata, data, renderdata.text[i], i);

        sre::render_draw1(renderdata.renderflags, data, m_atlas.get());

        topleft_begin.x += glyph.advance;
    }
}    