#pragma once
#include <ECS/camera.hpp>

#include <Datatypes/Vector.hpp>
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
        sre::Connection m_eventconnection;
        const sre::Action& m_action;

        static void handle_event(void* signalData, DragControl* self, sre::Event ev);
        static sre::Action default_action;
    };
}