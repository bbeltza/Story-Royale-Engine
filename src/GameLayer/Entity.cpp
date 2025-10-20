#include "Game/World.hpp"
#include "Game/Entity.hpp"
#include "Game/Component.hpp"

#include "Base/Display.hpp"

#include "config.h"

struct component_vtable
{
    static inline const component_vtable* get(const Game::Component* component) { return *(component_vtable**)component; }
    void* destructor;
    void* render;
    //void* update;
    void* pupdate;
    void* query;
};
#define check_vtablefor(func)   auto vtable = component_vtable::get(component); \
                                if (vtable->render == BASE_VTABLE->func) continue;

static const Game::Component BASE_COMP;
static const component_vtable* BASE_VTABLE = component_vtable::get(&BASE_COMP);

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
        check_vtablefor(render)
        if (componentDisabled(*component)) continue;
        component->Render(this);
    }
}

void Game::Entity::call_pupdate(TimeStamp dt)
{
    *const_cast<Vector2f*>(&lastVelocity) = Position; // This isn't really stored in read-only memory, so it's more likely safe
    for (auto component : m_Components)
    {
        check_vtablefor(pupdate)
        if (componentDisabled(*component)) continue;
        component->pUpdate(this, dt);
    }

    *const_cast<Vector2f*>(&lastVelocity) = Position.getSub(lastVelocity);

    pUpdate(dt);
}

bool Game::Entity::call_query(float* pt)
{
    for (auto component : m_Components)
    {
        check_vtablefor(query)
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