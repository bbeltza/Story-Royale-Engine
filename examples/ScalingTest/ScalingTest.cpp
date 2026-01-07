#include <Base/Display.hpp>
#include <Base/Runtime.hpp>
#include <Base/Input.hpp>
#include <Entry.h>

void Render()
{
    sre::vec2ut mpos = Input::MouseScreenPosition();
    mpos.println();
    Display::DrawRotatedRectangle({mpos.x, mpos.y, 10, 10}, 45, sre::col4::WHITE, Display::M_FILL, DISPLAY_DONT_CENTER);
}

void sre::initialize()
{
    beforeRender.Connect(Render, nullptr);
}
