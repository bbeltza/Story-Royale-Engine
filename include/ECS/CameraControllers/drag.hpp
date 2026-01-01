#pragma once
#include <ECS/camera.hpp>

#include <datatypes/vector.hpp>
#include <Events/Mouse.hpp>
#include <Events/Touch.hpp>
#include <Base/Signal.hpp>
#include <Base/Action.hpp>

namespace sreECS
{
    class DragControl: public CameraController
    {
    public:
        DragControl(const sre::Action& action=default_action);
        ~DragControl();

        sre::unit smoothness = 0.0_ut;
    protected:
        void on_update(Camera& camera) override;
        void on_pupdate(Camera& camera) override;
    private:
        sre::vec2ut m_camSpeed;
        sre::vec2ut m_lastmouseDelta;
        ConnectionHandle m_mouseConnection;
        ConnectionHandle m_touchConnection;
        const sre::Action& m_action;

        static void mouseMoveCallback(void*, DragControl*, const MouseMove*);
        static void touchMotionCallback(void*, DragControl*, const TouchFinger*);
        static sre::Action default_action;
    };
}