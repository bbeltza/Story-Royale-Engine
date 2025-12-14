#pragma once
#include "datatypes/vector.hpp"
#include "datatypes/flags.hpp"

namespace Window
{
    void ChangeIcon(const char* path);
    void SetFramerate(unsigned short FPS);
    void ToggleFullscreen();

    void Show();
    void Hide();
    void Focus();

    bool IsFullScreen();
    bool IsHidden();
}