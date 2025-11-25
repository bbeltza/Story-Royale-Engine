#include <standard>
#include "Game/World.hpp"
#include "Game/Component.hpp"
#include "Game/Entity.hpp"

#include "Engine.hpp"

#include "config.h"

#include "../internal.h"

Game::World* Game::World::s_TargetWorld = nullptr;

bool Game::World::cmp(const Entity* first, const Entity* second)
{
    bool res = first->zIndex < second->zIndex;
    return res;
}

Game::World::World() {}

Game::World::~World()
{
    if (engine.current_world == this) engine.current_world = nullptr;
    
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
        entity->pUpdated.Fire(dt);
    }

    CurrentCamera.Update(dt);
    pUpdated.Fire(dt);
}

void Game::World::call_update(TimeStamp dt)
{
    Update(dt);
    for (Entity* entity : getEntities<Entity>())
    {
        entity->Update(dt);
        entity->Updated.Fire(dt);
    }

    Updated.Fire(dt);
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

//

void Game::World::call_render()
{
    m_Entities.sort(cmp);

    preRender();
    for (Entity* entity : m_Entities)
    {
        entity->preRender();
        entity->call_render();
        entity->postRender();
        entity->Rendered.Fire();
    }
    postRender();

#ifdef DRAW_ENTITY_CENTER
    for (Entity* entity : m_Entities) entity->_debugDraw();
#endif

    Rendered.Fire();
}

Game::Camera* Game::World::currentCamera() { return engine.current_world ? &currworld->CurrentCamera : NULL; }

void Game::World::set(Game::World* world)
{
    if (engine.current_world) currworld->Destroy();
    engine.current_world = world;
}

Game::World* Game::World::current() { return currworld; }

static const Game::Camera zero_cam;

Vector2f Game::World::screenToWorld(float x, float y, const Camera* cam)
{
    if (!cam) cam = &zero_cam;
    return {x - engine.center_x + cam->rx, y - engine.center_y + cam->ry};
}

Vector2f Game::World::worldToScreen(float x, float y, const Camera* cam)
{
    if (!cam) cam = &zero_cam;
    return {x + engine.center_x - cam->rx , y + engine.center_y - cam->ry};
}