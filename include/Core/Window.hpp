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
}

#endif