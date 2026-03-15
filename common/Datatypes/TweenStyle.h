#ifndef SRE_TWEENSTYLE_H
#define SRE_TWEENSTYLE_H

#include <Datatypes/TimeStamp.h>
#include <utils/math.h>

typedef enum
{
    SRE_S_LINEAR,
    SRE_S_QUAD,
    SRE_S_CUBIC,
} sre_easingStyle;

typedef enum
{
    SRE_D_IN,
    SRE_D_OUT,
    SRE_D_INOUT
} sre_easingDirection;

typedef enum
{
    SRE_F_REVERSE = ut_bit(0)
} sre_tweenStyleFlags;

typedef struct
{
    sre_timeStamp duration;
    sre_timeStamp delay; // Unimplemented yet

    sre_easingStyle easing_style; // Animation easing is still not implemented, thus S_LINEAR is the only available option
    sre_easingDirection easing_direction;

    int repeat; // Unimplemented

    sre_tweenStyleFlags additional_flags; // F_REVERSE flag unimplemented too...
} sre_TweenStyle;

#ifdef __cplusplus
    namespace sre
    {
        using TweenStyle = sre_TweenStyle;
    }
#endif

#endif