#ifndef SRE_WINDOW_HPP
#define SRE_WINDOW_HPP
#include <Datatypes/Vector.h>
#include <Datatypes/Flags.hpp>

namespace sre
{
    bool window_togglefullscreen();

    void window_show();
    void window_hide();
    void window_focus();

    void window_minimize();
    void window_maximize();

    void window_setsize(int w, int h);
    void window_setresizable(bool resizable);

    bool window_flash();
    bool window_quickflash();
    bool window_cancelflash();

    bool window_isfullscreen();
    bool window_ishidden();

    // Functions from, formerly Display.hpp (I'm noting this here since I'm also going to change this header sometime)
    sre::vec2i  window_getsize(void);
    sre::vec2ut window_getviewport(void);
    sre::unit   window_getscale(void);
    sre::unit   window_getscale_ratio(void); // Return 1 / window_getscale(), this is only there since the engine caches that value for inputs

    bool window_set_manualscale(int scale); // Only call this if autoscaling is disable, autoscaling will override the window's scale
    bool window_enable_autoscaling(int target_w, int target_h);
    void window_disable_autoscaling(void);
}

#endif