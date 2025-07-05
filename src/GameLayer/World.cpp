#include <standard.h>
#include "ECS.h"
#include "Engine.h"

#include "Components.h"

Game::World* Game::currentWorld = nullptr;
Game::World* Game::World::s_TargetEntityWorld = nullptr;
unsigned int Game::World::center[2];

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
    m_Entities.sort(re_order);
    Engine->Window.getScreenCenter(this->center, this->center + 1);
    for (Entity* entity : this->m_Entities)
    {
        entity->preRender();
        entity->_render();
        entity->postRender();
    }

#ifdef _DEBUG
    for (Entity* entity : this->m_Entities) entity->_debugDraw();
#endif
}