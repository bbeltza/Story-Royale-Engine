#include <Core/Runtime.hpp>
#include <Core/Display.hpp>
#include <Core/Window.hpp>
#include <Core/Draw.hpp>
#include <Core/Input.hpp>

#include <Entry.h>

void Render()
{
    static double t = 0;
    sre::vec2ut mpos = sre::mouse_screencoords();
    
    sre::draw(sre::DDRRect{
        {
            SRE_DRAWFLAGS_STROKE & 0,
            sre::col4::WHITE,
            { mpos, {10, 10} },
            sre::vec2ut::CENTER
        },
        t
    });

    t += 1;
}

void sre::initialize()
{
    sre::window_setresizable(true);
    sre::display_autoscale_on(380, 180);

    beforeRender.connect(Render, nullptr);
}
