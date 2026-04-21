#include <Core/Runtime.hpp>
#include <Core/Display.hpp>
#include <Core/Window.hpp>
#include <Core/Render.h>
#include <Core/Input.hpp>

#include <Entry.h>

void Render()
{
    static float t = 0;
    sre::vec2ut mpos = sre::mouse_screencoords();
    
    sre::render_draw1(SRE_DRAWFLAG_LINE, { {
                { mpos, {10, 10} },
                sre::vec2ut::CENTER,
                sre::WHITE,
                t
        } });

    t += UT_PI/360;
}

void sre::initialize()
{
    sre::window_setresizable(true);
    sre::display_autoscale_on(380, 180);

    beforeRender.connect(Render, nullptr);
}
