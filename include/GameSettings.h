#pragma once
#include <stdint.h>
#include "Datatypes/Vector.h"

struct WindowOptions
{
    bool VSync;
    bool Resizable;
};

struct AudioOptions
{
};

namespace GameSettings
{
    extern const char* const Title;
    extern Vector2u const StartResolution;
    extern Vector2u const ScalingResolution;

    extern uint16_t const TargetFPS;
    
    extern ::WindowOptions const WindowOptions;
    extern ::AudioOptions const AudioOptions;
}