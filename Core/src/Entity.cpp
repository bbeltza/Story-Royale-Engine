#include "ECS.h"
#include "Components.h"
#include "Window.h"
#include "System.h"

Game::Entity* Game::Entity::s_targetEntityComponent = nullptr;
unsigned char Game::Entity::s_targetComponentType = NULLCOMPONENT;

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

void* Game::Entity::pushComponent(ENUM_ComponentType type, void **ptr)
{
    this->s_targetEntityComponent = this;
    this->s_targetComponentType = type;
    
    void* newComp = nullptr;

    switch (type)
    {
    case SHAPE:
        newComp = new Components::Shape();
        break;
    case VELOCITY:
        newComp = new Components::Velocity();
        break;
    case ENTITY_CONTAINER:
        break;
    default:
        this->s_targetEntityComponent = nullptr;
        sys_errorf("Couldn't create component, type doesn't exist: %u", type);
        break;
    }

    this->s_targetEntityComponent = nullptr;
    this->s_targetComponentType = NULLCOMPONENT;

    if (ptr) *ptr = newComp;
    return newComp;
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

