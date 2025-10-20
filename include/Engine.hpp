#pragma once

#include "Base/Audio.hpp"
#include "Base/Display.hpp"
#include "Base/Input.hpp"
#include "Base/Window.hpp"

namespace Runtime
{
    extern Signal<TimeStamp> OnUpdate;
    extern Signal<> BeforeRender;
    extern Signal<> AfterRender;

    intptr_t CurrentFrame();
}

#include "GameEntry.h"