#include "ECS.h"
#include "Components.h"
#include "Window.h"
#include "System.h"

Game::Entity::Entity()
{
    this->m_ParentType = WORLD;
    this->m_world = World::s_TargetEntityWorld;

    this->m_world->m_Entities.push_back(this);
}

Game::Entity::~Entity()
{
    m_world->m_Entities.remove(this);

    while (!this->m_Components.empty())
        this->popComponent();
}

Game::Components::Velocity* Game::Entity::addVelocityComponent()
{
    if (velocityComp)
    {
        printf("Entity already has a Velocity component\n");
        return nullptr;
    }

    velocityComp = new Components::Velocity;
    m_Components.push_back(velocityComp);

    return velocityComp;
}

void Game::Entity::popComponent()
{
    this->m_Components.pop_back();
}

void Game::Entity::reParent(World* const _world)
{
    m_world->m_Entities.remove(this);
    _world->m_Entities.push_back(this);
    m_world = _world;
}

void Game::Entity::_render()
{
    for (Component *component : this->m_Components)
    {
        if (component->getProcessFlags() & Component::p_Render)
            component->Render(this);
    }
}