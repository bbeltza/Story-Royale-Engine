#pragma once
#include "datatypes/vector.hpp"
#include "datatypes/rect.hpp"
#include "datatypes/color.hpp"

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

    sre::vec2ut GetCenter();
    sre::vec2i GetSize();
    sre::vec2i GetAbsoluteSize();

    float GetScale();

    void Fill(const sre::col4& Color);

    void DrawLine(const sre::col4& Color, const sre::vec2ut& Pt1, const sre::vec2ut& Pt2, const Game::World* world);
    void DrawLines(const sre::col4& Color, int Count, const sre::vec2ut* Pts, const Game::World* world);
    template <typename... _Args> void DrawLines(const sre::col4& Color, const Game::World* world, _Args&... Pts)
    {
        std::vector<sre::vec2ut> vec = {Pts...};
        DrawLines(Color, sizeof...(Pts), vec.data(), world);
    }

    void DrawRectangle(const sre::rect2Dut& Rectangle, const sre::col4& Color, const sre::vec2f& AnchorPoint, DrawingMode Mode, const Game::World* world);
    void DrawRotatedRectangle(const sre::rect2Dut& _Rectangle, const double _angle, const sre::col4& _Col, DrawingMode _mode, const Game::World* world);

    void DrawCircle(const sre::vec2ut& _Pos, const sre::unit _Radius, const sre::col4& _Col, DrawingMode _mode, const Game::World* world);

    void DrawDebug(const sre::vec2ut& pos);
}