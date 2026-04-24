#include <Core/Runtime.hpp>
#include <Core/Window.hpp>
#include <Core/Render.h>
#include <Base/Font.hpp>
#include <Entry.h>

const sre::RenderInstance1 bounds = {
    {10, 10, 120, 30},
    sre::vec2ut::ZERO,
    {255, 0, 0, 128}
};

void render()
{
    static sre::Font font("res://fonts/OpenSans-Regular.ttf", 12);
    sre::render_draw1(0, &bounds, 1);
    font.render(bounds.rectangle, sre::WHITE,"Hello world from Font::Render()", -1, sre::A_CENTER, sre::A_CENTER);
}

void sre::initialize()
{
    sre::window_setsize(200, 200);
    sre::beforeRender.connect(render, nullptr);
}