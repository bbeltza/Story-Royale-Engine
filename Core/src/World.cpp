#include <iostream>
#include "ECS.h"
#include "Window.h"

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


void Game::World::popEntity()
{
    delete this->m_Entities.back();
    this->m_Entities.pop_back();
}

void Game::World::removeEntity(unsigned int index)
{
    delete this->m_Entities[index];
    this->m_Entities.erase(m_Entities.begin() + index);
}
void Game::World::removeEntity(void* address)
{
    for (auto i = 0; i < this->m_Entities.size(); i++)
    {
        if (this->m_Entities[i] == address)
        {
            this->removeEntity(i);
            break;
        }
            
    }
}