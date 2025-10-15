#include "Game/World.hpp"
#include "Game/Entity.hpp"
#include "Game/Component.hpp"
#include "Base/Window.hpp"


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
        if (componentDisabled(*component)) continue;
        component->Render(this);
    }
}

void Game::Entity::call_pupdate(TimeStamp dt)
{
    *const_cast<Vector2f*>(&lastVelocity) = Position;
    for (auto c : m_Components)
    {
        if (componentDisabled(*c)) continue;
        c->pUpdate(this, dt);
    }

    *const_cast<Vector2f*>(&lastVelocity) = Position.getSub(lastVelocity);

    return pUpdate(dt);
}