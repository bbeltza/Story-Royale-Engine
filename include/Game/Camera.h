#pragma once
#include "Datatypes/Rect.h"

namespace Game
{
    struct Camera
    {
        enum ScalingType
        {
            Single,
            Multiple
        };

        float x = 0, y = 0;
        RectF clamp = {0, 0, 0, 0};
        ScalingType scalingType = Multiple;

        union
        {
            double Scale = 1;
            struct
            {
                float ScaleX, ScaleY;
            };
        };
    };
}