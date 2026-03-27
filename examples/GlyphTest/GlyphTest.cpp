#include <Core/Runtime.hpp>
#include <Core/Window.hpp>
#include <Core/Draw.hpp>
#include <Base/Font.hpp>
#include <Entry.h>

const sre::DDRect bounds = {
    SRE_DRAWFLAGS_STROKE,
    {255, 0, 0, 128},

    {10, 10, 120, 30},
    sre::vec2ut::ZERO
};

void render()
{
    static sre::Font font("res://fonts/OpenSans-Regular.ttf", 12);
    font.render(bounds.rect, sre::WHITE,"Hello world from Font::Render()", -1, sre::A_CENTER, sre::A_CENTER);
    sre::draw(bounds);
}

void sre::initialize()
{
    sre::window_setsize(140, 50);
    sre::beforeRender.connect(render, nullptr);
}