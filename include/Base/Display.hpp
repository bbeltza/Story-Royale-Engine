#pragma once
#include "datatypes/vector.hpp"
#include "datatypes/rect.hpp"
#include "datatypes/color.hpp"

namespace sreECS
{
    struct Scene;
}

namespace sre
{
    int draw();
}

#define DISPLAY_DONT_CENTER reinterpret_cast<const sreECS::Scene*>(0x01)

namespace Display
{
    enum DrawingMode
    {
        M_STROKE,
        M_FILL
    };

    sre::vec2ut GetCenter();
    sre::vec2i GetSize();
    sre::vec2i GetAbsoluteSize();

    float GetScale();

    void Fill(const sre::col4& Color);

    void DrawLine(const sre::col4& Color, const sre::vec2ut& Pt1, const sre::vec2ut& Pt2, const sreECS::Scene* world);
    void DrawLines(const sre::col4& Color, int Count, const sre::vec2ut* Pts, const sreECS::Scene* world);
    template <typename... _Args> void DrawLines(const sre::col4& Color, const sreECS::Scene* world, _Args&... Pts)
    {
        sre::vec2ut _pts[] = {Pts...};
        DrawLines(Color, sizeof...(Pts), _pts, world);
    }

    void DrawRectangle(const sre::rect2Dut& Rectangle, const sre::col4& Color, const sre::vec2f& AnchorPoint, DrawingMode Mode, const sreECS::Scene* world);
    void DrawRotatedRectangle(const sre::rect2Dut& _Rectangle, const double _angle, const sre::col4& _Col, DrawingMode _mode, const sreECS::Scene* world);

    void DrawCircle(const sre::vec2ut& _Pos, const sre::unit _Radius, const sre::col4& _Col, DrawingMode _mode, const sreECS::Scene* world);
}