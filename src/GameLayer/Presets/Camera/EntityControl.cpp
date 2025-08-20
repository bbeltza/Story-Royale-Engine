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
    getCamera()->x = m_Entity->Position.X;
    getCamera()->y = m_Entity->Position.Y;
}