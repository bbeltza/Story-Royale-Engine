#include <ECS/Layer.hpp>
#include <ECS/Scene.hpp>
#include <ECS/Entity.hpp>
#include <ECS/Component.hpp>

#include <Core/Runtime.hpp>
#include <Core/Input.hpp>

#include <Base/Error.h>
#include <Base/Log.h>

#include <algorithm>

sre::vec2ut sreECS::mouse_worldcoords() {
    sre::vec2ut pt{sre::mouse_screencoords()};
    pt = sre::process_input_coordinates(pt, 0, 0);

    return sreECS::get_current()->camera.toWorldSpace(pt);
}

using namespace sreECS;

Scene::~Scene()
{
    for (Entity& ent : *this)
        ent.destroy();
    
    if (m_attachedlyr)
        m_attachedlyr->m_current = NULL;
    m_attachedlyr = NULL;
}

Entity* Scene::add_child(Entity* entity)
{
    if (!entity) // Scene::add_child(NULL) is a no-op
        return NULL;

    if (entity->m_parent)
    {
        sre::error(SRE_ERR_UNAVAILABLE, "add_child(NULL) with an already parented entity.");
        return NULL;
    }    

    entity->m_parent = this;
    m_entities.push_back(entity);
    return entity;
}

//

void sreECS::set_physics_dt(sre::timeStamp dt, Layer* lyr)
{
    if (!lyr)
        lyr = sreECS::get_default_layer();
    assert(lyr != NULL);

    lyr->set_physics_dt(dt);
}

sre::timeStamp sreECS::get_physics_dt(Layer* lyr)
{
    if (!lyr)
        lyr = sreECS::get_default_layer();
    assert(lyr != NULL);

    return lyr->get_physics_dt();
}

void sreECS::set_current(Scene* scene, Layer* lyr)
{
    if (!lyr)
        lyr = sreECS::get_default_layer();
    assert(lyr != NULL);

    lyr->set_current(scene);
}

Scene* sreECS::get_current(Layer* lyr)
{
    if (!lyr)
        lyr = sreECS::get_default_layer();
    assert(lyr != NULL);

    return lyr->get_current();
}

//

sre::vec2ut Scene::process_camera()
{
    if (!camera.effect || !camera.effect->enabled)
        return camera.position;

    return camera.position + camera.effect->on_process(sre::dt);
}

void Scene::call_update()
{
    update();
    for (auto& ent : *this)
    {
        for (auto& comp : ent)
        {
            if (comp.enabled())
                comp.on_update(ent);
        }
    }
    camera.update();

    updated.fire();
}

void Scene::call_pupdate(sre::timeStamp dt)
{
    pupdate(dt);
    for (auto& ent : *this)
    {
        const_cast<sre::vec2ut&>(ent.lastVelocity) = ent.position;

        for (auto& comp : ent)
        {
            if (comp.enabled())
                comp.on_pupdate(ent, dt);
        }

        const_cast<sre::vec2ut&>(ent.lastVelocity) = ent.position - ent.lastVelocity;
    }
    camera.pupdate(dt);
}

void Scene::call_render()
{
    //
    std::sort(m_entities.begin(), m_entities.end(), [](Entity* e1, Entity* e2) {
        return e1->z_index < e2->z_index;
    });

    this->pre_render();

    for (auto& ent : *this)
    {      
        for (auto& comp : ent)
        {
            if (comp.enabled())
                comp.on_render(ent);
        }
    }

    this->post_render();

    rendered.fire();
}

bool Scene::call_query(sre::vec2ut coords)
{
    m_querying = NULL;

    for (auto it = rbegin(); it != rend(); ++it)
    {
        auto& entity = *it;
        for (auto& comp : entity)
        {
            if (comp.on_query(entity, coords))
            {
                m_querying = &entity;
                return true;
            }
        }
    }

    return false;
}
