#pragma once
#include <stdint.h>
#include "Datatypes/Vector.h"

namespace GameSettings
{
    extern const char* Title;
    extern Vector2u StartResolution;
    extern Vector2u ScalingResolution;

    extern uint16_t targetFPS;
    extern uint8_t windowFlags;
    extern uint8_t renderMode;
}