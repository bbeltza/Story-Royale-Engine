#pragma once
#include "Game/CameraControllers/API.h"

#include "Datatypes/Vector.h"

#include "Events/Mouse.h"

#include "Classes/Signal.h"

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

        static Vector2f s_lastmouseDelta;
        static Connection* s_mouseConnection;
        static void mouseMoveCallback(const MouseMove*);
    };
}