#ifndef SRE_DRAW_HPP
#define SRE_DRAW_HPP

#include <datatypes/rect.hpp>
#include <datatypes/color.hpp>

namespace sreECS
{
    typedef struct World World;
}

namespace sre
{
    struct CommonDrawData
    {
        const sreECS::World* world;
        sre::col4 color;
        int mode;
    };
    struct FillDrawData
    {
        sre::col4 color;
    };
    struct LineDrawData: CommonDrawData
    {
        sre::vec2ut pt1;
        sre::vec2ut pt2;
    };
    struct LinesDrawData: CommonDrawData
    {
        size_t count;
        const sre::vec2ut* pts;
    };
    struct RectDrawData: CommonDrawData
    {
        sre::rect2Dut rectangle;
        sre::vec2ut anchor;
    };
    struct RotRectDrawData : RectDrawData
    {
        double angle;
    };
}

namespace sre
{
    enum DrawType
    {
        DRAW_FILL,
        DRAW_LINE,
        DRAW_LINES,
        DRAW_RECTANGLE,
        DRAW_RRECTANGLE,
        DRAW_CIRCLE, // Unavailable
        DRAW_TEXTURE
    };
    enum DrawMode
    {
        DRAW_MFILL,
        DRAW_MSTROKE
    };

    int draw(int type, const void* data);

    extern const sreECS::World *DRAW_DONT_CENTER;

    template <typename... _Args>
    int draw_lines(const CommonDrawData& common, _Args&... Pts)
    {
        sre::vec2ut _pts[] = { Pts... };
        LinesDrawData data = common;
        data.pts = _pts;
        data.count = sizeof...(Pts);

        return draw(DRAW_LINES, &common);
    }
}

#endif