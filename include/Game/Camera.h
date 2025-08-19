#pragma once
#include "Datatypes/Rect.h"

#include "Game/CameraController.h"

namespace Game
{
    struct Camera
    {
        friend class World;
        friend class CameraController;

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

        inline CameraController* getCameraController() const { return m_Controller; }

        private:
        CameraController* m_Controller;

        void Update(TimeStamp);
    };
}