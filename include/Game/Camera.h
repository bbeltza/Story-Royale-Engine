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

        inline CameraController* getController() const { return m_Controller; }
        inline void setController(CameraController* controller) { m_Controller = controller; m_Controller->m_Camera = this; }

        private:
        CameraController* m_Controller = nullptr;

        void Update(TimeStamp);
    };
}