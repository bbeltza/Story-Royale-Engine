#include <ECS/CameraControllers/drag.hpp>
#include <Base/Display.hpp>
#include <Base/Runtime.hpp>

using namespace sreECS;

sre::Action DragControl::default_action{ sre::MB_RIGHT };

DragControl::DragControl(const sre::Action& action): m_action(action)
{
    m_mouseConnection = Input::MouseMove.Connect(DragControl::mouseMoveCallback, this);
    m_touchConnection = Input::FingerMove.Connect(DragControl::touchMotionCallback, this);
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

void DragControl::mouseMoveCallback(void*, DragControl* self, MouseMove event)
{
    if (self->m_action.pressed())
        self->m_lastmouseDelta += event.delta;
}

void DragControl::touchMotionCallback(void*, DragControl* self, TouchFinger event)
{
    sre::vec2ut screen = sre::display_size();

    self->m_lastmouseDelta += event.Delta * screen;
}