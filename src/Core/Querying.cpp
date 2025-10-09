#include "Base/Input.hpp"

#include "Game/World.hpp"
#include "Game/Entity.hpp"
#include "Game/Component.hpp"

#include "Game/GuiLayer.hpp"
#include "Game/GuiObject.hpp"

#include "Game/Components/Shape.hpp"

static Game::GuiObject *queryObject = nullptr;
static Game::Entity *queryEntity = nullptr;

static InputClass::_mState *mState;

void InputClass::queryObjects()
{
    mState = &m_mouseState;

    queryEntity = nullptr;
    queryObject = nullptr;

    queryObject = Game::GuiLayer::Current() ? Game::GuiLayer::Current()->_query() : 0;
    if (queryObject)
        return;
    queryEntity = Game::World::m_Current ? Game::World::m_Current->_query() : 0;
}

bool Game::GuiObject::isHovering() const
{
    return queryObject == this;
}

// _query() functions for both UI and World bases

Game::GuiObject *Game::GuiContainer::_query()
{
    GuiObject *target_return = nullptr;

    for (auto it = m_children.rbegin(); it != m_children.rend(); it++)
    {
        auto obj = *it;
        if (!obj->canQuery)
            continue;
        target_return = obj->_query();
        if (target_return) return target_return;
    }

    if (!isGuiLayer())
    {
        SDL_FPoint mousePoint = {mState->x, mState->y};
        SDL_FRect r{m_absolute.Position.X, m_absolute.Position.Y, m_absolute.Size.X, m_absolute.Size.Y};
        if (SDL_PointInFRect(&mousePoint, &r))
            target_return = (GuiObject *)this;
    }

    return target_return;
}

Game::Entity *Game::World::_query()
{
    Entity *target_returnEntity = nullptr;

    for (auto it = m_Entities.rbegin(); it != m_Entities.rend(); it++)
    {
        auto entity = *it;
        for (auto component : entity->m_Components)
        {
            if (entity->componentDisabled(*component)) continue;
            if (!component->hasProcessFlag(component->p_Query)) continue;
            if (component->Query(entity))
            {
                target_returnEntity = entity;
                break;
            }
        }
    }

    return target_returnEntity;
}

bool Components::Shape::Query(Game::Entity *_entity)
{
    return isInScreenPoint(_entity, {mState->x, mState->y});
}