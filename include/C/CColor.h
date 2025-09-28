#ifndef COLOR_H
#define COLOR_H
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

    extern const Color3 COLOR3_WHITE;
    extern const Color4 COLOR4_WHITE;
#if __cplusplus
}
#endif

#endif // COLOR_H