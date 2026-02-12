#include <ECS/CameraControllers/Drag.hpp>
#include <Core/Display.hpp>
#include <Core/Runtime.hpp>

using namespace sreECS;

sre::Action DragControl::default_action{ sre::MB_RIGHT };

DragControl::DragControl(const sre::Action& action): m_action(action), m_eventconnection(sre::onEvent.connect(handle_event, this))
{
}

DragControl::~DragControl() {}

void DragControl::on_update(Camera& camera)
{
    if (m_action.pressed())
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

void DragControl::on_pupdate(Camera&)
{
    // Empty.
}

void DragControl::handle_event(void*, DragControl* self, sre::Event ev)
{
    switch (ev.type)
    {
    case sre::EVENT_MOUSEMOVE:
        if (self->m_action.pressed()) self->m_lastmouseDelta += ev.mouse_move.delta;
        break;
    case sre::EVENT_TOUCH:
        self->m_lastmouseDelta += ev.touch.delta * sre::display_size();
        break;
    }
}