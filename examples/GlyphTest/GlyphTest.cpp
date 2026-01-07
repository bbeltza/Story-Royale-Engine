#include <Base/Runtime.hpp>
#include <Base/Display.hpp>
#include <Base/Font.hpp>
#include <Entry.h>

const sre::rect2Dut bounds(10, 10, 120, 30);

void render()
{
    static sre::Font font("res://fonts/OpenSans-Regular.ttf", 12);
    font.render(bounds, sre::col4::WHITE,"Hello world from Font::Render()", -1, sre::A_CENTER, sre::A_CENTER);
    Display::DrawRectangle(bounds, {255, 0, 0, 128}, sre::vec2f::ZERO, Display::M_STROKE, DISPLAY_DONT_CENTER);
}

void sre::initialize()
{
    sre::beforeRender.Connect(render, nullptr);
}