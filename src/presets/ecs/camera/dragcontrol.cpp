#include <ECS/CameraControllers/Drag.hpp>
#include <Core/Display.hpp>
#include <Core/Runtime.hpp>

using namespace sreECS;

const sre::Action DragControl::default_action = SRE_MAKE_ACTION(
    { sre::ACTION_MOUSE, sre::MB_RIGHT },
    { sre::ACTION_TOUCH, 0 } // `0` for now since there are no parameters for touch actions
);

DragControl::DragControl(const sre::Action& action): m_action(action), m_eventconnection(sre::onEvent.connect(handle_event, this))
{
}

DragControl::~DragControl() {}

void DragControl::on_update(Camera& camera)
{
    if (sre::action_pressed(m_action))
    {
        m_camSpeed = m_lastmouseDelta;
    }
    else if (smoothness <= 0.0_ut)
        m_camSpeed = m_camSpeed.ZERO;
    else
        m_camSpeed.setlerp(m_camSpeed.ZERO, ut_min(sre::dt / smoothness, 1.0_ut));

    camera.position -= m_camSpeed;
    m_lastmouseDelta = m_lastmouseDelta.ZERO;
}

void DragControl::on_pupdate(Camera&, sre::timeStamp dt)
{
    // Empty.
}

void DragControl::handle_event(void*, DragControl* self, sre::Event ev)
{
    switch (ev.type())
    {
    using namespace sre::events;
    case sre::EVENT_MOUSEMOVE:
        if (sre::action_pressed(self->m_action)) self->m_lastmouseDelta += ev.get<MouseMove>().delta;
        break;
    case sre::EVENT_TOUCH:
        self->m_lastmouseDelta += ev.get<Touch>().delta * sre::display_size();
        break;
    default:
        break;
    }
}