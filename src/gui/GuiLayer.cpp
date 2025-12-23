#include "Engine.hpp"

#include "Game/GuiLayer.hpp"
#include "Game/GuiObject.hpp"
#include "Game/GuiComponent.hpp"

#include "../internal.h"

Game::GuiLayer::~GuiLayer()
{
    if (engine.current_guilayer == this)
        engine.current_guilayer = nullptr;
}

void Game::GuiLayer::set(GuiLayer* layer)
{
    if (engine.current_guilayer)
        currlayer->destroy();
    engine.current_guilayer = layer;
}

Game::GuiLayer* Game::GuiLayer::curr() { return currlayer; }

Game::GuiContainer *Game::GuiContainer::s_targetParentContainer = nullptr;

Game::GuiContainer::GuiContainer() : m_parent(s_targetParentContainer),
                                     m_absolute{0, 0, 0, 0}
{
}

Game::GuiContainer::~GuiContainer()
{
    if (m_parent)
        m_parent->m_children.remove(reinterpret_cast<GuiObject *>(this));
    
    while (!m_children.empty())
        delete m_children.back();
}

void Game::GuiContainer::_processchildren()
{
    m_children.sort(GuiObject::cmp);
    uint32_t i = 0;
    for (GuiObject *obj : m_children)
    {
        if (!obj->visible)
            continue;

        obj->_process();
        obj->_processchildren();
        obj->_procpos_components();
        obj->_proc_children_components(i++);
    }
}

void Game::GuiObject::_process()
{
    // Process the size of the UI objects first

    m_absolute.size = size.to_absolute(m_parent->m_absolute.size);

    // Then transforming it with any components

    _procsize_components();

    // And finally process the positions with the transformed size

    m_absolute.position = m_parent->m_absolute.position + position.to_absolute(m_parent->m_absolute.size) - m_absolute.size * anchor;
}

void Game::GuiContainer::_renderchildren()
{
    _prerender_components();
    _render_components();

    #if 0
        Color4 r{255, 0, 0, 255};
        Display::DrawRectangle(m_absolute, r, Vector2f::ZERO, Display::dm_Stroke, DISPLAY_DONT_CENTER);
    #endif
    for (GuiContainer *obj : m_children)
    {
        if (!obj->visible)
            continue;

        obj->_renderchildren();
    }
    rendered.Fire();
}

void Game::GuiContainer::call_update()
{
    update();

    for (auto obj : m_children)
    {
        obj->call_update();
    }
    updated.Fire();
}

/*
Game::GuiComponent::GuiComponent() {}

Game::GuiComponent::~GuiComponent() {}
*/

void Game::GuiContainer::_prerender_components()
{
    if (m_parent)
        m_modulate = m_parent->m_modulate;
    else
        m_modulate = {255, 255, 255, 255};

    for (GuiComponent *comp : m_components)
    {
        if (comp->enabled)
            comp->pre_render(this);
    }
}

void Game::GuiContainer::_render_components()
{
    for (GuiComponent *comp : m_components)
    {
        if (comp->enabled)
            comp->render(this);
    }
}

void Game::GuiContainer::_procpos_components()
{
    for (GuiComponent *comp : m_components)
    {
        if (comp->enabled)
            comp->process_position(this);
    }
}

void Game::GuiContainer::_procsize_components()
{
    for (GuiComponent *comp : m_components)
    {
        if (comp->enabled)
            comp->process_size(this);
    }
}

void Game::GuiContainer::_proc_children_components(uint32_t index)
{
    for (GuiComponent *comp : m_parent->m_components)
        if (comp->enabled)
            comp->process_children((GuiObject *)this, index);
}

#define checktw(src, i, prop, t) if (src) {             \
    if (tweens[i]) delete tweens[i];                     \
    tweens[i] = new Tween<t>(Info, prop, *src);           \
    tweens[i]->Play();                                     \
    lastTw = tweens[i];                                     \
}

Signal<>* Game::GuiObject::TweenPosition(const TweenInfo& Info, const sre::udim::unit_type* XScale, const sre::udim::unit_type* XOffset, const sre::udim::unit_type* YScale, const sre::udim::unit_type* YOffset)
{
    TweenBase* lastTw = nullptr;
    checktw(XScale, 0, position.x.scale, sre::udim::unit_type)
    checktw(XOffset, 1, position.x.offset, sre::udim::unit_type)
    checktw(YScale, 2, position.y.scale, sre::udim::unit_type)
    checktw(YOffset, 3, position.y.offset, sre::udim::unit_type)

    return lastTw ? &lastTw->Completed : nullptr;
}

Signal<>* Game::GuiObject::TweenSize(const TweenInfo& Info, const sre::udim::unit_type* XScale, const sre::udim::unit_type* XOffset, const sre::udim::unit_type* YScale, const sre::udim::unit_type* YOffset)
{
    TweenBase* lastTw = nullptr;
    checktw(XScale, 4, size.x.scale, sre::udim::unit_type)
    checktw(XOffset, 5, size.x.offset, sre::udim::unit_type)
    checktw(YScale, 6, size.y.scale, sre::udim::unit_type)
    checktw(YOffset, 7, size.y.offset, sre::udim::unit_type)

    return lastTw ? &lastTw->Completed : nullptr;
}

Signal<>* Game::GuiObject::TweenAnchor(const TweenInfo& Info, const float* X, const float* Y)
{
    TweenBase* lastTw = nullptr;
    checktw(X, 8, anchor.x, float)
    checktw(Y, 9, anchor.y, float)

    return lastTw ? &lastTw->Completed : nullptr;
}

Game::GuiObject::~GuiObject()
{
    for (int i=0; i < 10; i++)
        if (tweens[i]) delete tweens[i];
}