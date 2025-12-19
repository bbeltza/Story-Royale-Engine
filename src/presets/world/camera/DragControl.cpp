#include "Game/CameraControllers/DragControl.hpp"

#include "Base/Input.hpp"
#include "Base/Display.hpp"

use_namespace

Action DragControl::default_action{ true, Input::mbRight };

DragControl::DragControl(const Action& action): m_action(action)
{
    m_mouseConnection = Input::MouseMove.Connect(DragControl::mouseMoveCallback, this);
    m_touchConnection = Input::FingerMove.Connect(DragControl::touchMotionCallback, this);
}

DragControl::~DragControl() {}

void DragControl::Update(sre::timeStamp delta)
{
    Camera& CurrentCamera = *getCamera();

    if (m_action.isPressed())
    {
        m_camSpeed = m_lastmouseDelta;
    }
    else if (Smoothness <= 0.0f)
        m_camSpeed = m_camSpeed.ZERO;
    else
        m_camSpeed.setlerp(m_camSpeed.ZERO, ut_min(delta / Smoothness, 1.0f));

    CurrentCamera.position -= m_camSpeed;
    m_lastmouseDelta = m_lastmouseDelta.ZERO;
}

void DragControl::pUpdate(sre::timeStamp delta)
{
    // Empty.
    (void)delta;
}

void DragControl::mouseMoveCallback(void*, DragControl* self, const MouseMove* event)
{
    if (self->m_action.isPressed())
        self->m_lastmouseDelta += event->delta;
}

void DragControl::touchMotionCallback(void*, DragControl* self, const TouchFinger* event)
{
    sre::vec2ut screen{Display::GetSize()};

    self->m_lastmouseDelta += event->Delta * screen;
}