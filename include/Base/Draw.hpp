#ifndef SRE_DRAW_HPP
#define SRE_DRAW_HPP

#include <Base/Draw.h>

#include <datatypes/rect.hpp>
#include <datatypes/color.hpp>

namespace sre
{
    using DDFill = sre_DDFill;
    using DDLine = sre_DDLine;
    using DDLines = sre_DDLines;
    using DDRect = sre_DDRect;
    using DDRRect = sre_DDRRect;
    using DDTexture = sre_DDTexture;
    using DDRTexture = sre_DDRTexture;

    inline int draw(const DDFill& data) { return sre_draw(SRE_DRAW_FILL, &data); }
    inline int draw(const DDLine& data) { return sre_draw(SRE_DRAW_LINE, &data); }
    inline int draw(const DDLines& data) { return sre_draw(SRE_DRAW_LINES, &data); }
    inline int draw(const DDRect& data) { return sre_draw(SRE_DRAW_RECTANGLE, &data); }
    inline int draw(const DDRRect& data) { return sre_draw(SRE_DRAW_RRECTANGLE, &data); }
    inline int draw(const DDTexture& data) { return sre_draw(SRE_DRAW_TEXTURE, &data); }
    inline int draw(const DDRTexture& data) { return sre_draw(SRE_DRAW_RTEXTURE, &data); }

    inline int draw_fill(const col4& color) { return sre_draw(SRE_DRAW_FILL, &color); }

    template <typename Args>
    inline int draw_lines(const col4& color, bool use_camera, Args&& pts_args...)
    {
        vec2ut pts[] = { pts_args... };
        return draw(DDLines{use_camera ? SRE_DRAWFLAGS_USECAM : 0, { color.r, color.g, color.b, color.a }, sizeof...(pts_args), 0, pts});
    }
}

#endif