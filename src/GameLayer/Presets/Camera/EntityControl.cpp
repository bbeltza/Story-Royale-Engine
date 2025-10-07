#include "Game/CameraControllers/EntityControl.hpp"
#include "utils.h"

use_namespace

EntityControl::~EntityControl()
{

}

void EntityControl::Update(TimeStamp dt)
{
    Camera& CurrentCamera = *getCamera();

    Vector2f diff = m_Target - m_Entity->Position;

    if (abs(diff.X) > Bounds.X)
        m_Target.X -= diff.X - Bounds.X * ut_sign(diff.X);
    if (abs(diff.Y) > Bounds.Y)
        m_Target.Y -= diff.Y - Bounds.Y * ut_sign(diff.Y);


    if (!Smoothness)
    {
        CurrentCamera.x = m_Target.X;
        CurrentCamera.y = m_Target.Y;
    }
    else
    {
        CurrentCamera.x = ut_lerp(CurrentCamera.x, m_Target.X, dt / Smoothness);
        CurrentCamera.y = ut_lerp(CurrentCamera.y, m_Target.Y, dt / Smoothness);
    }
    
}