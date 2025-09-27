#include "Game/CameraControllers/DragControl.h"

#include "Engine.h"

use_namespace

DragControl::DragControl()
{
    m_mouseConnection = Engine->Input.mouseMove.Connect(event_callback(DragControl::mouseMoveCallback), this);
}

DragControl::~DragControl()
{
    m_mouseConnection->Disconnect();
}

void DragControl::Update(TimeStamp delta)
{
    Camera& CurrentCamera = *getCamera();

    if (Engine->Input.isMouseButtonPressed(InputClass::mbRight))
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
    if (event->state & SDL_BUTTON(InputClass::mbRight))
    {
        self->m_lastmouseDelta.X += event->delta.X;
        self->m_lastmouseDelta.Y += event->delta.Y;
    }
}