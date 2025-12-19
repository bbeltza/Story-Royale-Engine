#include <standard>
#include "Game/World.hpp"
#include "Game/Component.hpp"
#include "Game/Entity.hpp"

#include "Engine.hpp"

#include "../internal.h"

Game::World *Game::World::s_TargetWorld = nullptr;

bool Game::World::cmp(const Entity *first, const Entity *second)
{
    bool res = first->zIndex < second->zIndex;
    return res;
}

Game::World::World() {}

Game::World::~World()
{
    if (engine.current_world == this)
        engine.current_world = nullptr;

    while (!m_Entities.empty())
    {
        delete m_Entities.back();
    }
}

void Game::World::call_pupdate()
{
    engine.phys_accum_dt += engine.last_dt;

    while (engine.phys_accum_dt > 0)
    {
        pUpdate(engine.phys_target_dt);
        for (Entity *entity : m_Entities)
        {
            entity->call_pupdate(engine.phys_target_dt);
            entity->pUpdated.Fire(engine.phys_target_dt);
        }
        CurrentCamera.pUpdate(engine.phys_target_dt);
        pUpdated.Fire(engine.phys_target_dt);
        
        engine.phys_accum_dt -= engine.phys_target_dt;
    }
    
}

void Game::World::call_update()
{
    Update(engine.last_dt);
    for (Entity *entity : getEntities<Entity>())
    {
        entity->Update(engine.last_dt);
        entity->Updated.Fire(engine.last_dt);
    }

    CurrentCamera.Update(engine.last_dt);
    Updated.Fire(engine.last_dt);
}

Game::Entity *Game::World::call_query(float *pt)
{
    for (auto it = m_Entities.rbegin(); it != m_Entities.rend(); ++it)
    {
        auto entity = *it;
        if (entity->call_query(pt))
            return entity;
    }

    return nullptr;
}

//

void Game::World::call_render()
{
    m_Entities.sort(cmp);

    preRender();
    for (Entity *entity : m_Entities)
    {
        entity->preRender();
        entity->call_render();
        entity->postRender();
        entity->Rendered.Fire();
    }
    postRender();

#ifdef DRAW_ENTITY_CENTER
    for (Entity *entity : m_Entities)
        entity->_debugDraw();
#endif

    Rendered.Fire();
}

Game::Camera *Game::World::currentCamera() { return engine.current_world ? &currworld->CurrentCamera : NULL; }

void Game::World::set(Game::World *world)
{
    if (engine.current_world)
        currworld->Destroy();
    engine.current_world = world;
}

Game::World *Game::World::current() { return currworld; }

static const Game::Camera zero_cam;

sre::vec2ut Game::World::screenToWorld(sre::unit x, sre::unit y, const Camera *cam)
{
    if (!cam)
        cam = &zero_cam;

    return sre::vec2ut{ x, y } - sre::vec2ut{engine.center_x, engine.center_y} + cam->rpos;
}

sre::vec2ut Game::World::worldToScreen(sre::unit x, sre::unit y, const Camera *cam)
{
    if (!cam)
        cam = &zero_cam;

    sre::vec2ut pos = sre::vec2ut{ x, y } + sre::vec2ut{engine.center_x, engine.center_y} - cam->rpos;
    pos.x = floor(pos.x * engine.current_scale);
    pos.y = floor(pos.y * engine.current_scale);

    return pos;
}