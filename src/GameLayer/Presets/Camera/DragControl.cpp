#include "Game/CameraControllers/DragControl.h"

#include "Engine.h"

use_namespace

Vector2f DragControl::s_lastmouseDelta;
Connection* DragControl::s_mouseConnection = nullptr;

DragControl::DragControl()
{
    if (!s_mouseConnection)
        s_mouseConnection = Engine->Input.mouseMove.Connect(event_callback(DragControl::mouseMoveCallback));
    s_mouseConnection->Reconnect();
}

DragControl::~DragControl()
{
    s_mouseConnection->Disconnect();
}

void DragControl::Update(TimeStamp delta)
{
    Camera& CurrentCamera = *getCamera();

    if (Engine->Input.isMouseButtonPressed(InputClass::mbRight))
    {
        m_camSpeed.X = s_lastmouseDelta.X;
        m_camSpeed.Y = s_lastmouseDelta.Y;
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
    
    s_lastmouseDelta.X = 0;
    s_lastmouseDelta.Y = 0;
}

void DragControl::mouseMoveCallback(const MouseMove* event)
{
    if (event->state & SDL_BUTTON(InputClass::mbRight))
    {
        s_lastmouseDelta.X += event->delta.X;
        s_lastmouseDelta.Y += event->delta.Y;
    }
}