#include <GUI/object.hpp>

#include "../internal.h"

using namespace sreGUI;

Object::Object()
{
    set_parent(m_parent);
}

Object::~Object()
{
    m_parent->children.remove(this);
    
    while (!children.empty())
        delete children.front();
}

void Object::set_parent(Object* parent)
{
    if (!m_parent) return;

    m_parent->children.remove(this);
    m_parent = parent;
    if (parent)
        parent->children.push_front(this);
    else
        engine.current_guilayer = this;
}