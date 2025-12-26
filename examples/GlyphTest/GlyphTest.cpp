#include <Engine.hpp>
#include <Base/Font.hpp>

Font* font;
const sre::rect2Dut bounds(10, 10, 120, 30);

void render()
{
    font->Render(bounds, "Hello world from Font::Render()", -1, Font::HCenter, Font::VCenter);
    Display::DrawRectangle(bounds, {255, 0, 0, 255}, sre::vec2f::ZERO, Display::M_STROKE, DISPLAY_DONT_CENTER);
}

void sre::initialize()
{
    static Font _font("res://fonts/OpenSans-Regular.ttf", 12);
    font = &_font;

    Runtime::BeforeRender.Connect(render, nullptr);
}