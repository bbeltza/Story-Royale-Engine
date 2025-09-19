#pragma once
#include <stdint.h>
#include "Datatypes/Vector.h"

struct WindowOptions
{
    bool VSync;
    bool Resizable;
    bool Hidden;
};

enum InterpolationMode
{
    Linear,
    Cubic,
    Gaussian
};

struct AudioOptions
{
    uint16_t Frequency;
    uint8_t Channels;
    uint8_t Interpolation;
};

namespace GameSettings
{
    extern const char *const Title;
    extern Vector2u const StartResolution;
    extern Vector2u const ScalingResolution;

    extern uint16_t const TargetFPS;

    extern ::WindowOptions const WindowOptions;
    extern ::AudioOptions const AudioOptions;
}
