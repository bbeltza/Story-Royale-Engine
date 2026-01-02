#include <GUI/object.hpp>
#include <GUI/component.hpp>
#include <Base/Display.hpp>

#include <utils/mem.h>

#include "../internal.h"

using namespace sreGUI;

Object::Object()
{
    if (!m_parent) return;

    m_parent->children.push_back(this);
}

Object::~Object()
{
    if (m_parent)
        m_parent->children.remove(this);
    
    while (!children.empty())
        delete children.front();
}

void Object::set_parent(Object* parent)
{
    if (!m_parent)
    {
        ERROR("Calling set_parent() on root Object, this is unimplemented");
        return;
    }
    if (!parent)
    {
        WARN("Calling set_parent(NULL), it is not implemented, use set_root() instead");
        return;
    }
    if (m_parent == parent) return;

    m_parent->children.remove(this);
    m_parent = parent;
    parent->children.push_back(this);
}

void Object::set_root(bool destroy_old)
{
    if (engine.current_guilayer == this)
    {
        WARN("Calling sreGUI::Object::set_rot() on root layer");
        return;
    }
    if (m_parent)
    {
        m_parent->children.remove(this);
        m_parent = NULL;
    }
    if (engine.current_guilayer)
        currlayer->destroy();
    
    engine.current_guilayer = this;
}

void Object::CContainer::setup(Component* const components[], size_t count)
{
    if (!components || !count)
    {
        ::operator delete(m_ptr);
        m_ptr = NULL;
        m_count = 0;
        return;
    }

    if (!m_ptr)
    {
        m_ptr = static_cast<Component**>(::operator new(sizeof(Component* const) * count));
        m_count = count;
    }
    else if(m_count != count)
    {
        ::operator delete(m_ptr);
        m_ptr = static_cast<Component**>(::operator new(sizeof(Component* const) * count));
        m_count = count;
    }

    for (size_t i = 0; i < m_count; i++)
        m_ptr[i] = components[i];
}

//

const Object *Object::call_query(sre::vec2ut pt) const
{
    const Object *target_return = nullptr;

    for (auto it = children.rbegin(); it != children.rend(); it++)
    {
        Object* obj = *it;
        if (!obj->flags.has(F_ENABLED))
            continue;
        if (!obj->flags.has(F_QUERY))
            continue;
        target_return = obj->call_query(pt);
        if (target_return) return target_return;
    }

    if (m_parent)
    {
        if (m_absolute.simple_intersects(pt))
            target_return = this;
    }

    return target_return;
}

void Object::call_process()
{
    for (auto& comp : components)
    {
        if (comp.enabled)
            m_absolute.size = comp.process_size(m_absolute);
    }
    for (auto& comp : components)
    {
        if (comp.enabled)
            m_absolute.position = comp.process_position(m_absolute, m_parent ? m_parent->m_absolute.size : sre::display_size());
    }
}

void Object::call_processchildren()
{
    children.sort([](const Object* o1, const Object* o2)
    {
        return o1->z_index < o2->z_index;
    });

    size_t children_count = children.size();
    size_t i = 0;
    ut_dynsalloc(sre::rect2Dut, arr, children_count);

    for (auto& obj : children)
    {
        if (!obj.flags.has(F_ENABLED)) continue;

        obj.m_absolute = m_absolute;
        obj.call_process();

        arr[i++] = obj.m_absolute;
    }

    for (auto& comp : components)
    {
        if (comp.enabled)
            comp.process_children(m_absolute, arr, children_count);
    }

    i = 0;

    for (auto& obj : children)
    {
        if (!obj.flags.has(F_ENABLED)) continue;

        obj.m_absolute = arr[i++];
        obj.call_processchildren();
        obj.call_prerender();
    }
}

void Object::call_prerender()
{
    for (auto& comp : components)
    {
        if (comp.enabled)   
            comp.on_prerender(m_absolute);
    }
}

void Object::call_update()
{
    update();
    for (auto& obj : children)
        obj.call_update();
    updated.Fire();
}

void Object::call_render()
{
    pre_render();

    for (auto& comp : components)
    {
        if (comp.enabled)
            comp.on_render(m_absolute);
    }
    
    for (auto& obj : children)
    {
        if (!obj.flags.has(F_ENABLED)) continue;

        obj.call_render();
    }
    post_render();

    /* 
    Display::DrawRectangle(m_absolute, sre::col4::RED, sre::vec2f::ZERO, Display::M_STROKE, DISPLAY_DONT_CENTER);
    */

    rendered.Fire();
}