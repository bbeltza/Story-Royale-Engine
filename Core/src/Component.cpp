#include "System.h"
#include "ECS.h"

Game::Component::Component(ENUM_ComponentType type) : m_type(type)
{
    this->parent = Entity::s_targetEntityComponent;
    this->parent->m_Components.push_back(this);

    this->initComponent();

    if (this->m_type == VELOCITY)
    {
        if (parent->velocityComp)
        {
            sys_error("Entity already has velocity component");
            parent->popComponent();
        }
        else
        {
            parent->velocityComp = this;
            std::cout << parent->velocityComp << "\n";
        }
    }

}

void Game::Component::initComponent()
{
    switch (this->m_type)
    {
    case ENUM_ComponentType::SHAPE:
        this->ShapeComponent = { 0, 0, 40, 40, {200, 200, 200, 255}, 0, VISIBLE };
        break;
    case ENUM_ComponentType::VELOCITY:
        this->VelocityComponent = { 0, 0 };
        break;
    case ENUM_ComponentType::ENTITY_CONTAINER:
        break;
    default:
        break;
    }
}

