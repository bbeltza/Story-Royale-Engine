#pragma once

#include "Base/Audio.hpp"
#include "Base/Display.hpp"
#include "Base/Input.hpp"
#include "Base/Window.hpp"

#include "event.hpp"

namespace Runtime
{
    extern Signal<sre::Event> onEvent;

    extern Signal<sre::timeStamp> OnUpdate;
    extern Signal<> BeforeRender;
    extern Signal<> AfterRender;

    intptr_t CurrentFrame();
    void SetFramerate(unsigned short FPS);
    void SetPhysicsRate(unsigned short FPS);

    extern const sre::timeStamp& delta_time;
    extern const sre::timeStamp& pdelta_time;
}

#include "GameEntry.h"