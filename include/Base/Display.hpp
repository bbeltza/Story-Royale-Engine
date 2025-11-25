#pragma once
#include "Datatypes/Vector.hpp"
#include "Datatypes/Rect.hpp"
#include "Datatypes/Color.hpp"

namespace Display
{
    enum DrawingMode
    {
        dm_Stroke,
        dm_Fill
    };

    Vector2f GetCenter();
    Vector2i GetSize();
    Vector2i GetAbsoluteSize();

    float GetScale();

    void Fill(const Color4& Color);

    void DrawLine(const Color4& Color, const Vector2f& Pt1, const Vector2f& Pt2);
    void DrawLines(const Color4& Color, int Count, const Vector2f* Pts);
    template <typename... _Args> void DrawLines(const Color4& Color, _Args&... Pts)
    {
        std::vector<Vector2f> vec = {Pts...};
        DrawLines(Color, sizeof...(Pts), vec.data());
    }

    void DrawRectangle(const RectF& Rectangle, const Color4& Color, const Color4& Modulate = Color4::WHITE, const Vector2f& AnchorPoint = Vector2f::CENTER, DrawingMode Mode = dm_Fill);
    void DrawRectangleAtWorld(RectF Rectangle, const Color4& Color, const Color4& Modulate = Color4::WHITE, const Vector2f& AnchorPoint = Vector2f::CENTER, DrawingMode Mode = dm_Fill);

    void DrawRotatedRectangle(const RectF& _Rectangle, const double _angle, const Color4& _Col, DrawingMode _mode = dm_Fill);
    void DrawRotatedRectangleAtWorld(const RectF& _Rectangle, const double _angle, const Color4& _Col, DrawingMode _mode = dm_Fill);

    void DrawCircle(const Vector2f& _Pos, const float _Radius, const Color4& _Col, DrawingMode _mode = dm_Fill);

    void DrawDebug(Vector2f pos);
}