#include <standard.h>
#include "Game/World.h"
#include "Game/Component.h"
#include "Game/Entity.h"

#include "Engine.h"
#include "System.h"

#include "config.h"

Vector2f Game::World::center;
Color3 Game::World::Background = {255, 149, 236};
Color4 Game::World::Foreground = {0};
Game::World* Game::World::m_Current = nullptr;
Game::World* Game::World::s_TargetWorld = nullptr;

static const char* const static_wTag = "WorldTag ;)";

bool Game::World::cmp(const Entity* first, const Entity* second)
{
    bool res = first->zIndex < second->zIndex;
    return res;
}

Game::World::World(): m_worldTag(static_wTag) {}

Game::World::~World()
{
    if (m_Current == this) m_Current = nullptr;
    
    while (!m_Entities.empty())
    {
        delete m_Entities.back();
    }
}

Game::Entity* Game::World::addEntity()
{
    return addEntity<Entity>();
}

Game::World* Game::World::checkCurrent(World* world)
{
    if (world->m_worldTag != static_wTag)
    {
        return nullptr;
    }

    m_Current = world;
    return world;
}

void Game::World::err()
{
    return System::Error(System::WORLD_CREATION_ERROR);
}

void Game::World::Update(TimeStamp dt)
{
    for (Entity* entity : getEntities<Entity>())
    {
        entity->Update(dt);
    }
}

void Game::World::call_render()
{
    center = Engine->DrawingContext.getScreenCenter();

    if (!m_Current) return;

    m_Current->m_Entities.sort(cmp);
    for (Entity* entity : m_Current->m_Entities)
    {
        entity->preRender();
        entity->call_render();
        entity->postRender();
    }

#ifdef DRAW_ENTITY_CENTER
    for (Entity* entity : Game::World::m_Current->m_Entities) entity->_debugDraw();
#endif
}