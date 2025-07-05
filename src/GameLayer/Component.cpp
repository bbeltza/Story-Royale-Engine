#include "System.h"
#include "ECS.h"

Game::Component::Component() : m_type((ENUM_ComponentType)Entity::s_targetComponentType), m_parent(Entity::s_targetEntityComponent)
{
    if (!m_type)
    {
        sys_error("Attempt at creating component without a type, please speficy a proper type");
        delete this;
    }

    this->m_parent->m_Components.push_back(this);

    if (this->m_type == VELOCITY)
    {
        if (m_parent->velocityComp)
        {
            m_parent->popComponent();
            sys_error("Entity already has velocity component");
        }
    }

}

Game::Component::~Component()
{
    this->m_parent->m_Components.remove(this);
}