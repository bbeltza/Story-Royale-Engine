#include <Base/Draw.hpp>
#include <Base/Runtime.hpp>
#include <Base/Input.hpp>
#include <Entry.h>

void Render()
{
    static double t = 0;
    sre::vec2ut mpos = sre::mouse_screencoords();
    mpos.println();
    
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
    beforeRender.connect(Render, nullptr);
}
