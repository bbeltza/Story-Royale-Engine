#include <Base/Display.hpp>
#include <Base/Input.hpp>
#include <Engine.hpp>

void Render()
{
    sre::vec2ut mpos = Input::MouseScreenPosition();
    mpos.println();
    Display::DrawRotatedRectangle({mpos.x, mpos.y, 10, 10}, 45, sre::col4::WHITE, Display::M_FILL, DISPLAY_DONT_CENTER);
}

void sre::initialize()
{
    Runtime::BeforeRender.Connect(Render, nullptr);
}
