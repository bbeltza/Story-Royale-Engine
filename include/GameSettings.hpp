#pragma once
#include <stdint.h>
#include <Datatypes/Vector.hpp>

struct WindowOptions
{
    bool VSync;
    bool Resizable;
    bool Hidden;
};

enum InterpolationMode
{
    Nearest,
    Linear,
    Cubic,
    Gaussian
};

struct AudioOptions
{
    uint16_t InputFrequency;
    uint16_t OutputFrequency;
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
