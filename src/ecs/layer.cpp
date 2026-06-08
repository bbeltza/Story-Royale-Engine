#include <ECS/Layer.hpp>
#include <ECS/Scene.hpp>

#include <Core/Runtime.hpp>
#include <Core/Render.h>
#include <Base/Log.h>

#include <Hints.h>

static int defaultindex = -1;
static sreECS::Layer* defaultlayer = NULL;
sreECS::Layer* sreECS::get_default_layer() {
    if (!defaultlayer)
        sre::log(SRE_LOG_WARN "Calling `sreECS::get_default_layer() before or after the engine's lifetime (returning NULL)`");
    return defaultlayer;
}

extern "C"
{
    SRE_EXPORT_HINT void SRE_HINT_ECS_ENTRYPOINT(void)
    {
        defaultlayer = new sreECS::Layer;
        defaultindex = sre::bind_object_layer(*defaultlayer);
    }
}

sreECS::Layer::Layer()
{
    // ...
}
sreECS::Layer::~Layer()
{
    if (m_current)
    {
        assert(m_current->m_attachedlyr == this);
        m_current->m_attachedlyr = NULL;
        m_current->destroy();
    }

    if (defaultlayer == this)
    {
        defaultindex = -1;
        defaultlayer = NULL;
    }
}

//

void sreECS::Layer::update()
{
    if (!m_current)
        return;

    m_current->call_update();
    if (m_pupdate_dt)
    {
        m_pupdate_accum += sre::dt;
        while (m_pupdate_accum > 0) {
            m_current->call_pupdate(m_pupdate_dt);
            m_pupdate_accum -= m_pupdate_dt;
        }
    }
    else
        m_current->call_pupdate(sre::dt);

    m_current->clamp_camera();
}

void sreECS::Layer::render()
{
    if (!m_current)
        return;

    sre::vec2ut campos = m_current->process_camera();
    if (!sre::render::has_begun())
        sre::render::begin(m_current->background, campos);

    m_current->call_render();
    if (m_current && m_current->foreground.a)
        sre::render::fill(m_current->foreground);
}

//

void sreECS::Layer::set_current(Scene* scene)
{
    const bool destroyold = true; // To be a parameter

    if (m_current)
    {
        m_current->m_attachedlyr = NULL;
        if (destroyold)
            m_current->destroy();
    }
    m_current = scene;
    if (scene)
    {
        scene->m_attachedlyr = this;
    }
}