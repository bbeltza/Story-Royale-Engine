#include <GUI/Layer.hpp>

#include <Core/Input.hpp>
#include <Core/Render.h>
#include <Core/Event.hpp>
#include <Core/Window.hpp>

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
        
        sre::vec2ut pt{sre::get_input_coordinates()};
        pt = sre::process_input_coordinates(pt, vp_area.position, vp_scale);

        hoveringstack.clear();
        cur->call_query(pt, hoveringstack);
        // Set the S_HOVERING state to the hovering object at the top
        // since the hovering object container holds `const` objects, we need to const_cast it.
        // There should not be any penalties on doing so
        if (!hoveringstack.empty())
            const_cast<sreGUI::Object*>(hoveringstack.back())->m_state.toggle_on(sreGUI::Object::S_HOVERING);

        cur->call_update();

        if (cur == root)
            break;
    }

    _displaycache = sre::calc_viewport_size(vp_area, vp_scale);

	root->m_absolute.size = _displaycache - sre::vec2ut{insets*2, 0};
	root->m_absolute.position = sre::vec2ut::ZERO + sre::vec2ut{insets, 0};

    root->call_process();
    root->call_processchildren();
    root->call_prerender(); // FIXME: Rename this to something like "post_process": The name pre_render is really confusing since it doesn't get called in the render
                                        // loop, considering that the virtual pre_render Object method gets called in the render loop.
}

void sreGUI::Layer::render()
{
    static constexpr sre::col4 LYR_BG = { 0, 0, 0, 255 }; // Normally black. It only has an effect when there's no ECS scenes
    static constexpr sre::vec2ut LYR_CAM = { 0, 0 }; // Always set to zero

    if (!sre::render::has_begun())
        sre::render::begin(LYR_BG, LYR_CAM);

    if (root)
    {
        assert(vp_scale >= 0);
        sre::render::set_viewport(vp_area, vp_scale);
        #if 0
            sre::render::fill({ 40, 40, 40, 255 });
        #endif
        root->call_render(this->scissors_stack);

        sre::render::reset_viewport();
    }
}