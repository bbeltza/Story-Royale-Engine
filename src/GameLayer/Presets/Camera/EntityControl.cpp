#include "Game/CameraControllers/EntityControl.h"

use_namespace

EntityControl::EntityControl(Entity& entity): m_Entity(&entity)
{

}

EntityControl::~EntityControl()
{

}

void EntityControl::Update(TimeStamp dt)
{
    Camera& CurrentCamera = *getCamera();

    CurrentCamera.x = m_Entity->Position.X;
    CurrentCamera.y = m_Entity->Position.Y;
}