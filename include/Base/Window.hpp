#pragma once
#include <SDL.hpp>

#include "Datatypes/Vector.hpp"
#include "Datatypes/Flags.hpp"

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