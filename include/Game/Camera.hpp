#pragma once
#include "Datatypes/Rect.hpp"

#include "Game/CameraController.hpp"

namespace Game
{
    struct Camera
    {
        friend class World;
        friend class CameraController;

        Unit x = 0;
        Unit y = 0;
        RectUt clamp = {0, 0, 0, 0};

        Unit ScaleX = 1;
        Unit ScaleY = 1;

        inline CameraController* getController() const { return m_Controller; }
        inline void setController(CameraController* controller) { m_Controller = controller; m_Controller->m_Camera = this; }

        private:
        CameraController* m_Controller = nullptr;

        void Update(TimeStamp);

        // Render positions
        Unit rx = 0;
        Unit ry = 0;
    };
}