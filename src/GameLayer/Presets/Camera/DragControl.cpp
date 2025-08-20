#include "Game/CameraControllers/DragControl.h"

#include "Engine.h"

use_namespace

Vector2f DragControl::s_lastmouseDelta;

DragControl::DragControl()
{

}

DragControl::~DragControl()
{

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
        m_camSpeed = m_camSpeed.Lerp(Vector2f::ZERO, delta / 0.2f);
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

Connection* const DragControl::s_mouseConnection = Engine->Input.mouseMove.Connect(event_callback(DragControl::mouseMoveCallback));