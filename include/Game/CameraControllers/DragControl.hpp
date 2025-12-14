#pragma once
#include "Game/CameraControllers/API"

#include "datatypes/vector.hpp"
#include "Events/Mouse.hpp"
#include "Events/Touch.hpp"
#include "Base/Signal.hpp"
#include "Base/Action.hpp"

namespace CameraControllers
{
    cam_controldef(DragControl)
    {
    public:
        DragControl(const Action& action=default_action);
        ~DragControl();

        float Smoothness = 0.0f;
    protected:
        void Update(sre::timeStamp) override;
        void pUpdate(sre::timeStamp) override;
    private:
        sre::vec2ut m_camSpeed;
        sre::vec2ut m_lastmouseDelta;
        ConnectionHandle m_mouseConnection;
        ConnectionHandle m_touchConnection;
        const Action& m_action;

        static void mouseMoveCallback(void*, DragControl*, const MouseMove*);
        static void touchMotionCallback(void*, DragControl*, const TouchFinger*);
        static Action default_action;
    };
}