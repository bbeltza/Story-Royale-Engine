#include "Base/Input.h"

#include "Game/World.h"
#include "Game/Entity.h"
#include "Game/Component.h"

#include "Game/GuiLayer.h"
#include "Game/GuiObject.h"

#include "Game/Components/Shape.h"

static Game::GuiObject *queryObject = nullptr;
static Game::Entity *queryEntity = nullptr;

static InputClass::_mState *mState;

void InputClass::queryObjects()
{
    mState = &m_mouseState;

    queryEntity = nullptr;
    queryObject = nullptr;

    queryObject = Game::GuiLayer::Current ? Game::GuiLayer::Current->_query() : 0;
    if (queryObject)
        return;
    queryEntity = Game::World::Current ? Game::World::Current->_query() : 0;
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
        if (target_return) break;
    }

    if (!target_return && !isGuiLayer())
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
        auto entity = reinterpret_cast<Entity *>(*it);
        for (auto component : entity->m_Components)
        {
            if (!component->hasProcessFlag(component->p_Query))
                continue;
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