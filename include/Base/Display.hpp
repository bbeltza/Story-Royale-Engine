#pragma once
#include "Datatypes/Vector.hpp"
#include "Datatypes/Rect.hpp"
#include "Datatypes/Color.hpp"

namespace Game
{
    class World;
}

#define DISPLAY_DONT_CENTER reinterpret_cast<Game::World*>(0x01)

namespace Display
{
    enum DrawingMode
    {
        dm_Stroke,
        dm_Fill
    };

    Vector2ut GetCenter();
    Vector2i GetSize();
    Vector2i GetAbsoluteSize();

    float GetScale();

    void Fill(const Color4& Color);

    void DrawLine(const Color4& Color, const Vector2ut& Pt1, const Vector2ut& Pt2, const Game::World* world);
    void DrawLines(const Color4& Color, int Count, const Vector2ut* Pts, const Game::World* world);
    template <typename... _Args> void DrawLines(const Color4& Color, const Game::World* world, _Args&... Pts)
    {
        std::vector<Vector2ut> vec = {Pts...};
        DrawLines(Color, sizeof...(Pts), vec.data(), world);
    }

    void DrawRectangle(const RectUt& Rectangle, const Color4& Color, const Vector2f& AnchorPoint, DrawingMode Mode, const Game::World* world);
    void DrawRotatedRectangle(const RectUt& _Rectangle, const double _angle, const Color4& _Col, DrawingMode _mode, const Game::World* world);

    void DrawCircle(const Vector2ut& _Pos, const Unit _Radius, const Color4& _Col, DrawingMode _mode, const Game::World* world);

    void DrawDebug(const Vector2ut& pos);
}