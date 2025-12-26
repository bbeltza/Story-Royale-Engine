#include <GUI/object.hpp>
#include <GUI/component.hpp>
#include <Base/Display.hpp>

#include <utils/mem.h>

#include "../internal.h"

using namespace sreGUI;

Object::Object()
{
    set_parent(m_parent);
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
        if (!parent)
            engine.current_guilayer = this;
        return;
    }

    m_parent->children.remove(this);
    m_parent = parent;
    if (parent)
        parent->children.push_front(this);
    else
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
        sre::rect2Dut centered{ m_absolute.bottom_right(), m_absolute.size };
        if (centered.intersects(pt))
            target_return = this;
    }

    return target_return;
}

void Object::call_process()
{
    children.sort([](const Object* o1, const Object* o2)
    {
        return o1->z_index < o2->z_index;
    });

    for (auto& comp : components)
        m_absolute.size = comp.process_size(m_absolute);
    for (auto& comp : components)
        m_absolute.position = comp.process_position(m_absolute, m_parent ? m_parent->m_absolute.size : sre::vec2ut(Display::GetSize()));

    const size_t count = children.size();
    ut_dynsalloc(sre::rect2Dut, arr, count);
    size_t i = 0;
    for (auto& obj : children)
    {
        if (!obj.flags.has(F_ENABLED)) continue;

        obj.m_absolute = m_absolute;
        obj.call_process();
        arr[i++] = obj.m_absolute;
    }

    i = 0;

    // Component::process_children stage
    for (auto& comp : components)
        comp.process_children(m_absolute, arr, count);
    for (auto& obj : children)
    {
        if (!obj.flags.has(F_ENABLED)) continue;
        obj.m_absolute = arr[i++];
    }
    
    for (auto& comp : components)
        comp.on_prerender(m_absolute);
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
        comp.on_render(m_absolute);
    
    for (auto& obj : children)
    {
        if (!obj.flags.has(F_ENABLED)) continue;

        obj.call_render();
    }
    post_render();

    rendered.Fire();
}