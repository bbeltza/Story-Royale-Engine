#include "ECS.h"

Game::Entity::Entity()
{
    this->m_ParentType = WORLD;
    this->m_world = World::s_TargetEntityWorld;

    this->m_world->m_Entities.push_back(this);
}

Game::Entity::~Entity()
{
    while (!this->m_Components.empty())
        this->popComponent();
}

void Game::Entity::popComponent()
{
    delete this->m_Components.back();
    this->m_Components.pop_back();
}

void Game::Entity::removeComponent(unsigned int index)
{
    delete this->m_Components[index];
    this->m_Components.erase(m_Components.begin() + index);
}

void Game::Entity::removeComponent(void* address)
{
    for (auto i = 0; i < this->m_Components.size(); i++)
    {
        if (this->m_Components[i] == address)
        {
            this->removeComponent(i);
            break;
        }
    }
}