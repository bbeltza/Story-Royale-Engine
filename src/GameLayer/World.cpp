#include <standard.h>
#include "Game/World.h"
#include "Game/Component.h"
#include "Game/Entity.h"

#include "Engine.h"

Vector2f Game::World::center;
Color3 Game::World::Background = {255, 149, 236};
Color4 Game::World::Foreground = {0};
Game::World* Game::World::Current = nullptr;
Game::World* Game::World::s_TargetWorld = nullptr;

bool Game::World::cmp(const Entity* first, const Entity* second)
{
    bool res = first->zIndex < second->zIndex;
    return res;
}

Game::World::World() {}

Game::World::~World()
{
    if (Current == this) Current = nullptr;
    
    while (!m_Entities.empty())
    {
        delete m_Entities.back();
    }
}

void Game::World::Update(delta_model dt)
{
    for (Entity* entity : getEntities<Entity>())
    {
        entity->Update(dt);
    }
}

void Game::World::call_render()
{
    center = Engine->DrawingContext.getScreenCenter();

    if (!Current) return;

    Current->m_Entities.sort(cmp);
    for (Entity* entity : Current->m_Entities)
    {
        entity->preRender();
        entity->call_render();
        entity->postRender();
    }

#ifdef _DEBUG
    for (Entity* entity : Game::World::Current->m_Entities) entity->_debugDraw();
#endif
}