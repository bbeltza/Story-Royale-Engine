#include <standard.h>
#include "ECS.h"
#include "Engine.h"

#include "Components.h"

Game::World* Game::currentWorld = nullptr;
Game::World* Game::World::s_TargetEntityWorld = nullptr;
int Game::World::center[2];

Game::World::World()
{
    std::cout << "World created!!\n";
}

Game::World::~World()
{
    std::cout << "World deleted!\n";
    while (!this->m_Entities.empty())
        this->popEntity();
}

void Game::World::Update(float dt)
{
    for (Entity* entity : m_Entities)
    {
        entity->Update(dt);
    }
}

void Game::World::render()
{
    Engine->DrawingContext.getScreenCenter((unsigned int*)center, (unsigned int*)center + 1);

    if (!Game::currentWorld) return;

    Game::currentWorld->m_Entities.sort(re_order);
    for (Entity* entity : Game::currentWorld->m_Entities)
    {
        entity->preRender();
        entity->_render();
        entity->postRender();
    }

#ifdef _DEBUG
    for (Entity* entity : Game::currentWorld->m_Entities) entity->_debugDraw();
#endif
}