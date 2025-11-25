#pragma once
#include "Datatypes/Rect.hpp"

#include "Game/CameraController.hpp"

namespace Game
{
    struct Camera
    {
        friend class World;
        friend class CameraController;

        float x = 0;
        float y = 0;
        RectF clamp = {0, 0, 0, 0};

        float ScaleX = 1;
        float ScaleY = 1;

        inline CameraController* getController() const { return m_Controller; }
        inline void setController(CameraController* controller) { m_Controller = controller; m_Controller->m_Camera = this; }

        private:
        CameraController* m_Controller = nullptr;

        void Update(TimeStamp);

        // Render positions
        float rx = 0;
        float ry = 0;
    };
}