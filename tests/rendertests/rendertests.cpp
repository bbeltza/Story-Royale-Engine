#include <Core/Runtime.hpp>
#include <Core/Defer.hpp>
#include <Core/Draw.hpp>
#include <Core/Window.hpp>
#include <Core/Display.hpp>
#include <Base/Log.h>

#include <Entry.h>

namespace
{
    using drawtype = int;
    drawtype currentpage = SRE_DRAW_FIRST;

    double angle_timer = 0;
    constexpr static sre::unit LINE_CLOCKNORM = 20;

    void callback_onrender()
    {
        switch (currentpage)
        {
            case SRE_DRAW_FILL:
                {   double col[3] = {0}; (void)col;
                    int color_timestamp = static_cast<int>(angle_timer*2)%7;
                    switch(color_timestamp)
                    {
                        case 0:
                            col[0] = 1;
                            break; 
                        case 1:
                            col[0] = 1;
                        case 2:
                            col[1] = 1;
                            break;
                        case 3:
                            col[1] = 1;
                        case 4:
                            col[2] = 1;
                            break;
                        case 5:
                            col[2] = 1;
                        case 6:
                            col[0] = 1;
                            break;
                    }

                    sre::draw_fill(sre::col4::fromNormalized(col[0], col[1], col[2], round(sin(angle_timer)*14+16)/32));

                } break;
            case SRE_DRAW_LINE:
                sre::draw(sre::DDLine{ // Draws a red line relative to the screen
                    0,
                    sre::RED,

                    { 40, 30 },
                    { 100.3, 30.3 }
                });

                sre::draw(sre::DDLine{ // Draws a sort of clock rotating blue line relative to the screen center
                    SRE_DRAWFLAGS_USECAM,
                    sre::BLUE,

                    { 0, 0 },
                    { cos(angle_timer)*LINE_CLOCKNORM, sin(angle_timer)*LINE_CLOCKNORM }
                });
                break;
            case SRE_DRAW_LINES:
                sre::draw_lines();
                break;
            default: abort();
        }

        angle_timer += sre::dt;
    }

    void callback_onevent(void*, void*, sre::Event ev)
    {
        switch (ev.type)
        {
            case sre::EVENT_KEYPRESS:
                if (ev.key_press.press != sre::KEY_PRESSED) break;
                switch (ev.key_press.scancode)
                {
                case sre::KBSCANCODE_E:
                    sre::log<sre::LOGCATEGORY_INFO>("Exiting success...");
                    sre::defer(exit, 0);
                    break;
                case sre::KBSCANCODE_Q:
                    sre::log<sre::LOGCATEGORY_INFO>("Exiting faillure...");
                    sre::defer(exit, -1);
                    break;
                case sre::KBSCANCODE_SPACE:
                    currentpage = (currentpage+1)%SRE_DRAWTYPES;
                    break;
                default: break;
                }
            default: break;
        }
    }
}


void sre::initialize()
{
    sre::window_setresizable(true);
    //sre::display_autoscale_on(120, 120);

    sre::log<sre::LOGCATEGORY_INFO>("CONTROLS:");
    sre::log<sre::LOGCATEGORY_INFO>("   Press E to return success");
    sre::log<sre::LOGCATEGORY_INFO>("   Press Q to return faillure");
    sre::log<sre::LOGCATEGORY_INFO>("   Press SPACE to switch between different rendering functions");

    sre::onEvent.connect(callback_onevent, NULL);
    sre::beforeRender.connect(callback_onrender, NULL);
}