#include <GUI/Layer.hpp>

#include <Core/Display.hpp>
#include <Core/Input.hpp>
#include <Core/Render.h>

#include <Base/Log.h>

#include <Hints.h>

static int defaultindex = -1;
static sreGUI::Layer* defaultlayer = NULL;
sreGUI::Layer* sreGUI::get_default_layer() {
    if (!defaultlayer)
        sre::log(SRE_LOG_WARN "Calling `sreECS::get_default_layer() before or after the engine's lifetime (returning NULL)`");
    return defaultlayer;
}

extern "C"
{
    SRE_EXPORT_HINT void SRE_HINT_GUI_ENTRYPOINT()
    {
        defaultlayer = new sreGUI::Layer;
        defaultindex = sre::bind_object_layer(*defaultlayer);
    }
}

sreGUI::Layer::Layer()
{
    // Nothing to do.
}
sreGUI::Layer::~Layer()
{
    if (root)
    {
        assert(root->m_attachedlyr == this);
        root->m_attachedlyr = NULL;
        root->destroy();
    }

    if (defaultlayer == this)
    {
        defaultindex = -1;
        defaultlayer = NULL;
    }
}

void sreGUI::Layer::set_root(sreGUI::Object* object)
{
    assert(!object || !object->m_parent);
    const bool destroyold = true;

    if (root)
    {
        root->m_attachedlyr = NULL;
        if (destroyold)
            root->destroy();
    }

    if (object)
    {
        object->m_attachedlyr = this;
    }
    root = object;
}

void sreGUI::Layer::update()
{    
    for (;;)
    {
        sreGUI::Object* cur = root;
        if (!cur)
            return;
        
        hoveringstack.clear();
        cur->call_query(sre::mouse_screencoords(), hoveringstack); // Gotta replace the sre::mouse_screencoords with something else. Input is very flawed in this engine
        // Set the S_HOVERING state to the hovering object at the top
        // since the hovering object container holds `const` objects, we need to const_cast it.
        // There should not be any penalties on doing so
        if (!hoveringstack.empty())
            const_cast<sreGUI::Object*>(hoveringstack.back())->m_state.toggle_on(sreGUI::Object::S_HOVERING);

        cur->call_update();

        if (cur == root)
            break;
    }

	root->m_absolute.size = sre::display_size() - sre::vec2ut{insets*2, 0};
	root->m_absolute.position = sre::vec2ut::ZERO + sre::vec2ut{insets, 0};

    root->call_process();
    root->call_processchildren();
    root->call_prerender();
}

void sreGUI::Layer::render()
{
    if (!sre::render::has_begun())
        sre::render::begin(sre::BLACK, sre::vec2ut::ZERO);

    if (root)
    {
        root->call_render(this->scissors_stack);
    }
}