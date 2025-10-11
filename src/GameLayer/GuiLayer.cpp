#include "Engine.hpp"

#include "Game/GuiLayer.hpp"
#include "Game/GuiObject.hpp"
#include "Game/GuiComponent.hpp"

#include "config.h"

Color4 Game::GuiLayer::Foreground = {0, 0, 0, 0};
Game::GuiLayer *Game::GuiLayer::m_Current = nullptr;

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

    m_absolute.Size.X = size.X.toAbsolute(m_parent->m_absolute.Size.X);
    m_absolute.Size.Y = size.Y.toAbsolute(m_parent->m_absolute.Size.Y);

    // Then transforming it with any components

    _procsize_components();

    // And finally process the positions with the transformed size

    m_absolute.Position.X = m_parent->m_absolute.Position.X + position.X.toAbsolute(m_parent->m_absolute.Size.X) - m_absolute.Size.X * anchor.X;
    m_absolute.Position.Y = m_parent->m_absolute.Position.Y + position.Y.toAbsolute(m_parent->m_absolute.Size.Y) - m_absolute.Size.Y * anchor.Y;
}

void Game::GuiContainer::_renderchildren()
{
    _prerender_components();
    _render_components();

    #ifdef DRAW_GUI_CONTAINER_BOX
        Color4 r{255, 0, 0, 255};
        Engine->DrawingContext.DrawRectangle(m_absolute, r, Color4::WHITE, Vector2f::ZERO, DrawingDevice::dm_Stroke);
    #endif
    for (GuiContainer *obj : m_children)
    {
        if (!obj->visible)
            continue;

        obj->_renderchildren();
    }
    Rendered.Fire(this);
}

void Game::GuiContainer::call_update(TimeStamp dt)
{
    Update(dt);

    for (auto obj : m_children)
    {
        if (obj->visible)
            obj->call_update(dt);
        else
            obj->Updated.Fire(this);
    }
    Updated.Fire(this);
}

Game::GuiComponent::GuiComponent() {}

Game::GuiComponent::~GuiComponent() {}

void Game::GuiContainer::_prerender_components()
{
    if (m_parent)
        m_modulate = m_parent->m_modulate;
    else
        m_modulate = {255, 255, 255, 255};

    for (GuiComponent *comp : m_components)
    {
        if (comp->hasFlag(GuiComponent::PRE_RENDER) && comp->enabled)
            comp->pre_render(this);
    }
}

void Game::GuiContainer::_render_components()
{
    for (GuiComponent *comp : m_components)
    {
        if (comp->hasFlag(GuiComponent::RENDER) && comp->enabled)
            comp->render(this);
    }
}

void Game::GuiContainer::_procpos_components()
{
    for (GuiComponent *comp : m_components)
    {
        if (comp->hasFlag(GuiComponent::PROCESS_POSITION) && comp->enabled)
            comp->process_position(this);
    }
}

void Game::GuiContainer::_procsize_components()
{
    for (GuiComponent *comp : m_components)
    {
        if (comp->hasFlag(GuiComponent::PROCESS_SIZE) && comp->enabled)
            comp->process_size(this);
    }
}

void Game::GuiContainer::_proc_children_components(uint32_t index)
{
    for (GuiComponent *comp : m_parent->m_components)
        if (comp->hasFlag(GuiComponent::PROCESS_CHILDREN) && comp->enabled)
            comp->process_children((GuiObject *)this, index);
}

#define checktw(src, i, prop, t) if (src) {             \
    if (tweens[i]) delete tweens[i];                     \
    tweens[i] = new Tween<t>(Info, prop, *src);   \
    tweens[i]->Play();                                     \
    lastTw = tweens[i];                                     \
}

Signal* Game::GuiObject::TweenPosition(TweenInfo& Info, const float* XScale, const int* XOffset, const float* YScale, const int* YOffset)
{
    TweenBase* lastTw = nullptr;
    checktw(XScale, 0, position.X.Scale, float)
    checktw(XOffset, 1, position.X.Offset, int)
    checktw(YScale, 2, position.Y.Scale, float)
    checktw(YOffset, 3, position.Y.Offset, int)

    return lastTw ? &lastTw->Completed : nullptr;
}

Signal* Game::GuiObject::TweenSize(TweenInfo& Info, const float* XScale, const int* XOffset, const float* YScale, const int* YOffset)
{
    TweenBase* lastTw = nullptr;
    checktw(XScale, 4, size.X.Scale, float)
    checktw(XOffset, 5, size.X.Offset, int)
    checktw(YScale, 6, size.Y.Scale, float)
    checktw(YOffset, 7, size.Y.Offset, int)

    return lastTw ? &lastTw->Completed : nullptr;
}

Signal* Game::GuiObject::TweenAnchor(TweenInfo& Info, const float* X, const float* Y)
{
    TweenBase* lastTw = nullptr;
    checktw(X, 8, anchor.X, float)
    checktw(Y, 9, anchor.Y, float)

    return lastTw ? &lastTw->Completed : nullptr;
}

Game::GuiObject::~GuiObject()
{
    for (int i=0; i < 10; i++)
        if (tweens[i]) delete tweens[i];
}