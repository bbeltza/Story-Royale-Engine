#pragma once
#include "Datatypes/Vector.hpp"
#include "Datatypes/Flags.hpp"

#include <Base/Image.hpp>

namespace sre
{
    bool window_seticon(const sre::Image& img);
    bool window_togglefullscreen();

    void window_show();
    void window_hide();
    void window_focus();

    void window_minimize();
    void window_maximize();

    bool window_flash();
    bool window_quickflash();
    bool window_cancelflash();

    bool window_isfullscreen();
    bool window_ishidden();
}