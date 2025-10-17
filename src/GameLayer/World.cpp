#include <standard>
#include "Game/World.hpp"
#include "Game/Component.hpp"
#include "Game/Entity.hpp"

#include "Engine.hpp"
#include "Sys.h"

#include "config.h"

Vector2f Game::World::center;
Color3 Game::World::Background = {255, 149, 236};
Color4 Game::World::Foreground = {0x00000000};
Game::World* Game::World::m_Current = nullptr;
Game::World* Game::World::s_TargetWorld = nullptr;

bool Game::World::cmp(const Entity* first, const Entity* second)
{
    bool res = first->zIndex < second->zIndex;
    return res;
}

Game::World::World() {}

Game::World::~World()
{
    if (m_Current == this) m_Current = nullptr;
    
    while (!m_Entities.empty())
    {
        delete m_Entities.back();
    }
}

void Game::World::call_pupdate(TimeStamp dt)
{
    pUpdate(dt);
    for (Entity *entity : m_Entities)
    {
        entity->call_pupdate(dt);
        entity->pUpdated.Fire(entity);
    }

    CurrentCamera.Update(dt);
    pUpdated.Fire(this);
}

void Game::World::call_update(TimeStamp dt)
{
    Update(dt);
    for (Entity* entity : getEntities<Entity>())
    {
        entity->Update(dt);
        entity->Updated.Fire();
    }

    Updated.Fire(this);
}

void Game::World::call_render()
{
    center = Engine->DrawingContext.getScreenCenter();

    if (!m_Current) return;

    m_Current->m_Entities.sort(cmp);

    m_Current->preRender();
    for (Entity* entity : m_Current->m_Entities)
    {
        entity->preRender();
        entity->call_render();
        entity->postRender();
        entity->Rendered.Fire(entity);
    }
    m_Current->postRender();

#ifdef DRAW_ENTITY_CENTER
    for (Entity* entity : Game::World::m_Current->m_Entities) entity->_debugDraw();
#endif

    m_Current->Rendered.Fire(m_Current);
}

Game::Entity* Game::World::call_query(float* pt)
{
    for (auto it = m_Entities.rbegin(); it != m_Entities.rend(); ++it)
    {
        auto entity = *it;
        if (entity->call_query(pt)) return entity;
    }

    return nullptr;
}