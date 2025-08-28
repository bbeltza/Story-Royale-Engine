#include <standard.h>
#include <SDL.hpp>

#include "Game/World.h"
#include "Game/Entity.h"
#include "Game/Component.h"
#include "Game/Components/Shape.h"

using namespace Components;

// Physics are temporarely unavaiable! Will be working on them...

void Game::Entity::_pProcess(TimeStamp dt)
{
    for (auto c : m_Components)
    {
        if (componentDisabled(c)) continue;
        if (c->hasProcessFlag(Component::p_pUpdate))
            c->pUpdate(this, dt);
    }
}

void Game::World::pUpdate(TimeStamp dt)
{
    for (Entity *entity : m_Entities)
    {
        entity->_pProcess(dt);
        entity->pUpdate(dt);
    }

    CurrentCamera.Update(dt);
}