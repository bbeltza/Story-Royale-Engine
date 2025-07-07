#pragma once
#include <stdint.h>

namespace GameSettings
{
    extern const char* Title;
    extern struct t2
    {
        unsigned int width;
        unsigned int height;
    } StartResolution;

    extern uint16_t targetFPS;
    extern uint8_t windowFlags;
    extern uint8_t renderMode;
}