#include "Game/World.hpp"
#include "Game/Entity.hpp"
#include "Game/Component.hpp"

#include "Base/Display.hpp"

#include "config.h"

Game::Entity::Entity(): m_ParentType(WorldParent), m_world(World::s_TargetWorld)
{
}

Game::Entity::~Entity()
{
    m_world->m_Entities.remove(this);
}

void Game::Entity::reParent(World* const _world)
{
    m_world->m_Entities.remove(this);
    _world->m_Entities.push_front(this);
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
    *const_cast<sre::vec2ut*>(&lastVelocity) = Position; // This isn't really stored in read-only memory, so it's more likely safe
    for (auto component : m_Components)
    {
        if (componentDisabled(*component)) continue;
        component->pUpdate(this, dt);
    }

    *const_cast<sre::vec2ut*>(&lastVelocity) = Position - lastVelocity;

    pUpdate(dt);
}

bool Game::Entity::call_query(float* pt)
{
    for (auto component : m_Components)
    {
        if (componentDisabled(*component)) continue;
        if (component->Query(this, pt)) return true;
    }

    return false;
}

void Game::Entity::_debugDraw()
{
#if DRAW_ENT_CENTER_ONLY_EMPTY
    if (this->m_Components.empty())
#endif
    {
        Display::DrawDebug(Position);
    }
}