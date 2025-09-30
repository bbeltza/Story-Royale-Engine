#include "Game/CameraControllers/EntityControl.hpp"
#include "utils.h"

use_namespace

EntityControl::~EntityControl()
{

}

void EntityControl::Update(TimeStamp dt)
{
    Camera& CurrentCamera = *getCamera();

    if (!Smoothness)
    {
        CurrentCamera.x = m_Entity->Position.X;
        CurrentCamera.y = m_Entity->Position.Y;
    }
    else
    {
        CurrentCamera.x = lerp(CurrentCamera.x, m_Entity->Position.X, dt / Smoothness);
        CurrentCamera.y = lerp(CurrentCamera.y, m_Entity->Position.Y, dt / Smoothness);
    }
}