#include "Input.h"
#include "ECS.h"
#include "GuiLayer.h"
#include "Components.h"

static Game::GuiObject* queryObject = nullptr;
static Game::Entity* queryEntity = nullptr;

static InputClass::_mState* mState;

void InputClass::queryObjects()
{
    mState = &m_mouseState;

    queryEntity = nullptr;
    queryObject = nullptr;

    queryObject = Game::currentGuiLayer ? Game::currentGuiLayer->_query() : 0;
    if (queryObject) return;
    queryEntity = Game::currentWorld ? Game::currentWorld->_query() : 0;
}

bool Game::GuiObject::isHovering() const
{
    return queryObject == this;
}

// _query() functions for both UI and World bases

Game::GuiObject* Game::GuiContainer::_query()
{
    GuiObject* target_return = nullptr;

    if (!p_children.empty())
    {
        auto i = p_children.end();
        i--;

        bool done = 0;
        while (!target_return)
        {
            if (i == p_children.begin()) done = 1;

            auto obj = *i;
            if (!obj) continue;
            if (!obj->canQuery) continue;
            target_return = obj->_query();

            if (done) break;
            i--;
        }
    }

    if (!target_return && !isGuiLayer())
    {
        SDL_FPoint mousePoint = { mState->x, mState->y };
        if (SDL_PointInFRect(&mousePoint, &p_absolute)) target_return = (GuiObject*)this;
    }

    return target_return;
}

Game::Entity* Game::World::_query()
{
    Entity* target_returnEntity = nullptr;

    if (!m_Entities.empty())
    {
        auto i = m_Entities.end();
        i--;

        bool done = 0;
        while (!target_returnEntity)
        {
            if (i == m_Entities.begin()) done = 1;

            auto entity = *i;

            for (auto _component : entity->m_Components)
            {
                if (!_component->getType() == SHAPE) continue;
                auto component = (Components::Shape*)_component;
                if (component->isInScreenPoint({ mState->x, mState->y }))
                {
                    target_returnEntity = entity;
                    break;
                }
            }

            if (done) break;
            i--;
        }
    }

    return target_returnEntity;
}