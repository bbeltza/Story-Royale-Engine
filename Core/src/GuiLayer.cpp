#include "GuiLayer.h"
#include "Window.h"
#include "ColorFunctions.h"

Game::GuiLayer* Game::currentGuiLayer = nullptr;
Game::GuiContainer* Game::GuiContainer::s_targetParentContainer = nullptr;
Game::GuiContainer* Game::GuiComponent::s_targetComponentParent = nullptr;

Game::GuiContainer::GuiContainer() : p_parent(s_targetParentContainer), p_absolute{ 0, 0, 0, 0 }
{
}

Game::GuiContainer::~GuiContainer()
{
    while (!p_children.empty()) popGuiObject();
    while (!p_components.empty()) popGuiComponent();
}

void Game::GuiContainer::_processchildren()
{
    if (!this->isGuiLayer())
    {
        _procpos_components();
        _proc_children_components();
    }

    for (GuiContainer* _cont : p_children)
    {
        auto obj = (GuiObject*)_cont;
        if (!obj->visible) continue;

        obj->_process();
        obj->_processchildren();
    }
}



void Game::GuiObject::_process()
{
    // Process the size of the UI objects first

    p_absolute.w = size.X.toAbsolute(p_parent->p_absolute.w);
    p_absolute.h = size.Y.toAbsolute(p_parent->p_absolute.h);

    // Then transforming it with any components

    _procsize_components();

    // And finally process the positions with the transformed size

    p_absolute.x = p_parent->p_absolute.x + position.X.toAbsolute(p_parent->p_absolute.w) - p_absolute.w * anchor.X;
    p_absolute.y = p_parent->p_absolute.y + position.Y.toAbsolute(p_parent->p_absolute.h) - p_absolute.h * anchor.Y;
}

void Game::GuiObject::_render()
{
    SDL_SetRenderDrawColorMod(Engine::Window::sdl_Renderer, &color, &p_modulate);
    SDL_RenderFillRectF(Engine::Window::sdl_Renderer, &p_absolute);
}

void Game::GuiContainer::_callUpdate(float dt)
{
    Update(dt);

    for (auto _obj : p_children)
    {
        auto obj = (Game::GuiObject*)_obj;
        if (obj->visible)
            obj->_callUpdate(dt);
    }
}

Game::GuiComponent::GuiComponent() :
    p_parent(s_targetComponentParent) {
}



void Game::GuiContainer::_prerender_components()
{
    if (p_parent) p_modulate = p_parent->p_modulate;
    else p_modulate = { 255, 255, 255, 255 };

    for (GuiComponent* comp : p_components)
    {
        if (comp->hasFlag(PRE_RENDER))
            comp->pre_render();
    }
}

void Game::GuiContainer::_render_components()
{
    for (GuiComponent* comp : p_components)
    {
        if (comp->hasFlag(RENDER))
            comp->render();
    }
}


void Game::GuiContainer::_procpos_components()
{
    for (GuiComponent* comp : p_components)
    {
        if (comp->hasFlag(PROCESS_POSITION))
            comp->process_position();
    }
}

void Game::GuiContainer::_procsize_components()
{
    for (GuiComponent* comp : p_components)
    {
        if (comp->hasFlag(PROCESS_SIZE))
            comp->process_size();
    }
}

void Game::GuiContainer::_proc_children_components()
{
    for (GuiComponent* comp : p_parent->p_components)
        if (comp->hasFlag(PROCESS_CHILDREN))
            comp->process_children((GuiObject*)this);
}