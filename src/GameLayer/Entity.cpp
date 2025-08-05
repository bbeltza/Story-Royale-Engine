#include "Game/World.h"
#include "Game/Entity.h"
#include "Game/Component.h"
#include "Base/Window.h"
#include "System.h"


Game::Entity::Entity(): m_ParentType(WorldParent), m_world(World::s_TargetWorld)
{
    m_world->m_Entities.push_back(this);
}

Game::Entity::~Entity()
{
    m_world->m_Entities.remove(this);
}

void Game::Entity::reParent(World* const _world)
{
    m_world->m_Entities.remove(this);
    _world->m_Entities.push_back(this);
    m_world = _world;
}

void Game::Entity::call_render()
{
    for (Component *component : this->m_Components)
    {
        if (component->hasProcessFlag(Component::p_Render))
            component->Render(this);
    }
}