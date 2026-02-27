#include <GUI/Object.hpp>
#include <GUI/Component.hpp>
#include <Core/Display.hpp>
#include <Core/Draw.hpp>

#include <utils/mem.h>

#include "../internal.h"

using namespace sreGUI;

Object* sreGUI::get_root() { return currlayer; }

const Object* Object::s_querying = NULL;

Object::Object()
{
    if (!m_parent)
    {
        // Root objects couldn't be queried before, so they are have the query flag disabled by default now as they can be hovered
        flags.toggle_off(F_QUERY);
        return;
    }

    m_parent->children.push_back(this);
}

Object::~Object()
{
    if (m_parent)
        m_parent->children.remove(this);
    
    while (!children.empty())
        delete children.front();
    
    if (engine.current_guilayer == this) engine.current_guilayer = NULL;
}

void Object::set_parent(Object* parent)
{
    if (engine.current_guilayer == this)
    {
        sre::log<sre::LOGCATEGORY_ERROR>("Calling set_parent() on root Object, this is unimplemented");
        return;
    }
    if (!parent)
    {
        sre::log<sre::LOGCATEGORY_WARN>("Calling set_parent(NULL), it is not implemented, use set_root() instead");
        return;
    }
    if (m_parent == parent) return;

    if (m_parent) m_parent->children.remove(this);
    m_parent = parent;
    parent->children.push_back(this);
}

void Object::set_root(bool destroy_old)
{
    if (engine.current_guilayer == this)
    {
        sre::log<sre::LOGCATEGORY_WARN>("Calling sreGUI::Object::set_rot() on root layer");
        return;
    }
    if (m_parent)
    {
        m_parent->children.remove(this);
        m_parent = NULL;
    }
    if (engine.current_guilayer && destroy_old)
        currlayer->destroy();
    
    engine.current_guilayer = this;
}

void Object::remove_root(bool destroy_old)
{
    if (engine.current_guilayer && destroy_old)
        currlayer->destroy();
    engine.current_guilayer = NULL;
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

bool Object::call_query(sre::vec2ut pt) const
{
    s_querying = NULL;
    if (!flags.has(F_ENABLED))
        return false;

    for (auto it = children.rbegin(); it != children.rend(); it++)
    {
        Object* obj = *it;

        if (obj->call_query(pt)) return true;
    }

    if (flags.has(F_QUERY) && m_absolute.simple_intersects(pt))
        return s_querying = this;

    return false;
}

void Object::call_process()
{
    for (auto& comp : components)
    {
        if (comp.enabled())
            m_absolute.size = comp.process_size(m_absolute);
    }
    for (auto& comp : components)
    {
        if (comp.enabled())
            m_absolute.position = comp.process_position(m_absolute, m_parent ? m_parent->m_absolute.size : sre::display_size());
    }
}

void Object::call_processchildren()
{
    children.sort([](const Object* o1, const Object* o2) { return o1->z_index < o2->z_index; });

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
        if (comp.enabled())
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
        if (comp.enabled())   
            comp.on_prerender(m_absolute);
    }
}

void Object::call_update()
{
    update();
    for (auto& obj : children)
        obj.call_update();
    updated.fire();
}

void Object::call_render()
{
    bool has_clip = flags.has(F_CLIP);

    pre_render();

    if (has_clip)
        sre_draw_clipbegin(&m_absolute);
    
    for (auto& comp : components)
    {
        if (comp.enabled())
            comp.on_render(m_absolute);
    }
    
    for (auto& obj : children)
    {
        if (!obj.flags.has(F_ENABLED)) continue;

        obj.call_render();
    }

    if (has_clip)
        sre_draw_clipend();

    post_render();

    /*
    sre::draw(sre::DDRect{
        SRE_DRAWFLAGS_STROKE,
        sre::col4::RED,

        m_absolute,
        sre::vec2ut::ZERO
    });
    */

    rendered.fire();
}