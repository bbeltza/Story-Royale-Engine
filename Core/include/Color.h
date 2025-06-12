#pragma once
#include <stdint.h>

#if __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        uint8_t r, g, b;
    } Color3;

    typedef struct
    {
        uint8_t r, g, b, a;
    } Color4;

#if __cplusplus
}
#endif