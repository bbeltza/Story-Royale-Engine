#include "Game/World.h"
#include "Game/Entity.h"
#include "Game/Component.h"
#include "Base/Window.h"


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
        if (componentDisabled(component)) continue;
        if (component->hasProcessFlag(Component::p_Render))
            component->Render(this);
    }
}

void Game::Entity::call_pupdate(TimeStamp dt)
{
    *const_cast<Vector2f*>(&lastVelocity) = Position;
    for (auto c : m_Components)
    {
        if (componentDisabled(c)) continue;
        if (c->hasProcessFlag(Component::p_pUpdate))
            c->pUpdate(this, dt);
    }

    *const_cast<Vector2f*>(&lastVelocity) = Position.getSub(lastVelocity);

    return pUpdate(dt);
}