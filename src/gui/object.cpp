#include <GUI/Layer.hpp>
#include <GUI/Object.hpp>
#include <GUI/Component.hpp>
#include <Core/Display.hpp>
#include <Core/Render.h>
#include <Base/Error.h>
#include <Base/Log.h>

#include <utils/mem.h>

using namespace sreGUI;

Object::Object(Object* parent, sreGUI::Component*const components[], size_t numcomponents)
{
    if (parent)
        set_parent(parent);

    if (components)
        this->components.setup(components, numcomponents);
}

Object::~Object()
{
    if (m_parent)
        m_parent->children.remove(this);
    
    while (!children.empty())
        children.front()->destroy();
    
    if (m_attachedlyr)
    {
        assert(m_attachedlyr->root == this);
        m_attachedlyr->root = NULL;
        m_attachedlyr = NULL;
    }  
}

Object* Object::set_parent(Object* parent)
{
    if (parent == this)
    {
        sre::error(SRE_ERR_INVALID_PARAMETER, "Calling set_parent() on itself. This is illegal behavior.");
        return NULL;
    }

    if (m_attachedlyr)
    {
        sre::error(SRE_ERR_INVALID_STATE, "Calling set_parent() on root Object, it's an impossible operation. Please, detach it from being the root object and proceed.");
        return this;
    }
    if (m_parent == parent)
        return this;
    if (!parent)
        sre::log(SRE_LOG_WARN "Calling set_parent(NULL) ; This is going to detach the object entirely from everything, so it is entirely yours!");

    if (m_parent)
        m_parent->children.remove(this);
    if (parent)
        parent->children.push_back(this);
    m_parent = parent;

    return this;
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
// Global GUI object functions (That wrap the layer ones)

const std::deque<const sreGUI::Object*>& sreGUI::get_hovering(Layer* lyr)
{
    if (!lyr)
        lyr = sreGUI::get_default_layer();
    assert(lyr != NULL);

    return lyr->get_hovering();
}

void sreGUI::set_root(Object* object, Layer* lyr)
{
    if (!lyr)
        lyr = sreGUI::get_default_layer();
    assert(lyr != NULL);

    lyr->set_root(object);
}
Object* sreGUI::get_root(Layer* lyr)
{
    if (!lyr)
        lyr = sreGUI::get_default_layer();
    assert(lyr != NULL);

    return lyr->get_root();
}

sre::unit sreGUI::get_insets(Layer* lyr)
{
    if (!lyr)
        lyr = sreGUI::get_default_layer();
    assert(lyr != NULL);

    return lyr->get_insets();
}
void sreGUI::set_insets(sre::unit insets, Layer* lyr)
{
    if (!lyr)
        lyr = sreGUI::get_default_layer();
    assert(lyr != NULL);

    lyr->set_insets(insets);
}

//

void Object::call_query(sre::vec2ut pt, std::deque<const Object*>& stack)
{
    m_state.toggle_off(S_INCURSOR | S_HOVERING);
    if (!flags.has(F_ENABLED))
        return;

    if (flags.has(F_QUERY) && m_absolute.simple_intersects(pt))
    {
        m_state.toggle_on(S_INCURSOR);
        stack.push_back(this);
    }

    for (auto& obj : children)
        obj.call_query(pt, stack);
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

void Object::call_render(sre::ClipStackUT& clipstack)
{
    if (!flags.has(F_ENABLED)) return;

    bool has_clip = flags.has(F_CLIP);

    this->pre_render();

    if (has_clip)
    {
        clipstack.push(m_absolute);
        sre::render::set_scissors(clipstack.top());
    }
    
    for (auto& comp : components)
    {
        if (comp.enabled())
            comp.on_render(m_absolute);
    }

    this->render();
    
    for (auto& obj : children)
        obj.call_render(clipstack);

    if (has_clip)
    {
        if (clipstack.pop())
            sre::render::set_scissors(clipstack.top());
        else
            sre::render::reset_scissors();
    }

    this->post_render();

    rendered.fire();

    /*
    sre::render::draw2(0, sre::RED, {
        m_absolute.position,
        m_absolute.position + sre::vec2ut{ m_absolute.size.x, 0 },
        m_absolute.position + m_absolute.size,
        m_absolute.position + sre::vec2ut{ 0, m_absolute.size.y }
    }, SRE_PRIMITIVE_LINELOOP);
    */
}