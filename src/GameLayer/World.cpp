#include <standard.h>
#include "Game/World.h"
#include "Game/Component.h"
#include "Game/Entity.h"

#include "Engine.h"

Vector2f Game::World::center;
Color3 Game::World::Background = {255, 149, 236};
Color4 Game::World::Foreground = {0};
Game::World* Game::World::Current = nullptr;

static inline bool entity_re_order(const Game::Entity *first, const Game::Entity *second) { return first->zIndex < second->zIndex; }

Game::World::World() {}

Game::World::~World()
{
    if (Current == this) Current = nullptr;
    while (!m_Entities.empty())
    {
        delete m_Entities.back();
        m_Entities.pop_back();
    }
}

void Game::World::Update(float dt)
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

    Current->m_Entities.sort(entity_re_order);
    for (Entity* entity : Current->getEntities<Entity>())
    {
        entity->preRender();
        entity->call_render();
        entity->postRender();
    }

#ifdef _DEBUG
    for (Entity* entity : Game::World::Current->m_Entities) entity->_debugDraw();
#endif
}