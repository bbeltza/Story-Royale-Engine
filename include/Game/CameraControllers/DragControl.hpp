#pragma once
#include "Game/CameraControllers/API"

#include "Datatypes/Vector.hpp"
#include "Classes/Signal.hpp"
#include "Events/Mouse.hpp"

namespace CameraControllers
{
    cam_controldef(DragControl)
    {
    public:
        DragControl();
        ~DragControl();

        float Smoothness = 0.0f;
    protected:
        void Update(TimeStamp) override;
    private:
        Vector2f m_camSpeed;
        Vector2f m_lastmouseDelta;
        Connection* m_mouseConnection;

        static void mouseMoveCallback(void*, DragControl*, const MouseMove*);
    };
}