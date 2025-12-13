#pragma once
#include "datatypes/rect.hpp"

#include "Game/CameraController.hpp"

namespace Game
{
    struct Camera
    {
        friend class World;
        friend class CameraController;

        sre::vec2ut position;
        sre::vec2ut scale;
        sre::rect2Dut clamp = {0, 0, 0, 0};

        inline CameraController* getController() const { return m_Controller; }
        inline void setController(CameraController* controller) { m_Controller = controller; m_Controller->m_Camera = this; }

        private:
        CameraController* m_Controller = nullptr;

        void Update(TimeStamp);
        void pUpdate(TimeStamp);

        // Render positions
        sre::vec2ut rpos;
    };
}