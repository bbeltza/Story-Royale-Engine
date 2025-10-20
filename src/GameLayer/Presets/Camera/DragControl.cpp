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

void DragControl::Update(TimeStamp delta)
{
    Camera& CurrentCamera = *getCamera();

    if (m_action.isPressed())
    {
        m_camSpeed.X = m_lastmouseDelta.X;
        m_camSpeed.Y = m_lastmouseDelta.Y;
    }
    else
    {
        if (Smoothness <= 0.0f)
            m_camSpeed = Vector2f::ZERO;
        else
        {
            float a = SDL_min(delta/Smoothness, 1.0f);
            m_camSpeed = m_camSpeed.Lerp(Vector2f::ZERO, a);
        }
    }
    
    CurrentCamera.x -= m_camSpeed.X;
    CurrentCamera.y -= m_camSpeed.Y;
    
    m_lastmouseDelta.X = 0;
    m_lastmouseDelta.Y = 0;
}

void DragControl::mouseMoveCallback(void*, DragControl* self, const MouseMove* event)
{
    if (event->state & SDL_BUTTON(Input::mbRight))
    {
        self->m_lastmouseDelta.X += event->delta.X;
        self->m_lastmouseDelta.Y += event->delta.Y;
    }
}

void DragControl::touchMotionCallback(void*, DragControl* self, const TouchFinger* event)
{
    Vector2f screen = Display::GetSize();

    self->m_lastmouseDelta.X += event->Delta.X * screen.X;
    self->m_lastmouseDelta.Y += event->Delta.Y * screen.Y;
}