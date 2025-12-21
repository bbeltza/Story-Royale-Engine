#include <ecs/scene.hpp>
#include <ecs/entity.hpp>

#include "../internal.h"
#include "../internal.hpp"

using namespace sreECS;

Scene::_Arena* Scene::new_arena()
{
    _Arena* buff = (_Arena*)operator new(sizeof(_Arena));
    buff->next = NULL;

    return buff;
}

Scene::Scene(): m_arenabuff(new_arena())
{
}

Scene::~Scene()
{
    for (Entity& ent : *this)
        ent.~Entity();

    do
    {
        _Arena* next = m_arenabuff->next;
        operator delete(m_arenabuff);
        m_arenabuff = next;
    } while (m_arenabuff);
    
}

void Scene::make_current(Scene* scene, bool destroy_old)
{
    if (engine.current_world && destroy_old)
        delete static_cast<Scene*>(engine.current_world);
    
    engine.current_world = scene;
}

Scene* Scene::current()
{
    return static_cast<Scene*>(engine.current_world);
}

Entity* Scene::alloc_entity(size_t size, size_t* _realsize)
{
    assert(size >= sizeof(Entity));

    Entity* result = NULL;

    size_t realsize = size;

    for (auto it = m_freelist.begin(); it != m_freelist.end(); it++)
    {
        auto& fl = *it;

        if (fl.size > realsize)
            continue;
        
        result = entity_at(fl.offset);
        result->m_offset = fl.offset;
        if (fl.size == realsize)
        {
            m_freelist.erase(it);
            it--;
        }
        else
        {
            fl.size -= realsize;
            fl.offset += realsize;
        }
    }

    if (result) goto setup_entity;

    if (m_entities.empty())
    {
        result = reinterpret_cast<Entity*>(m_arenabuff->data);
        result->m_offset = 0;
        m_entities.push_back(0);
    }
    else
    {
        size_t backoffset = m_entities.back();
        size_t accumulate = 0;
        _Arena* curr = m_arenabuff;
        while (backoffset > _Arena::SIZE)
        {
            backoffset -= _Arena::SIZE;
            accumulate += _Arena::SIZE;
            curr = curr->next;
        }
        
        size_t backsize = reinterpret_cast<Entity*>(curr->data + backoffset)->m_size;
        size_t nextoffset = backoffset + backsize;
        if (nextoffset + realsize > _Arena::SIZE)
        {
            curr->next = new_arena();
            curr = curr->next;
            nextoffset = 0;
            accumulate += _Arena::SIZE;
        }
        result = reinterpret_cast<Entity*>(curr->data + nextoffset);
        result->m_offset = nextoffset + accumulate;
        m_entities.push_back(result->m_offset);
    }


    setup_entity:

    if (_realsize) *_realsize = realsize;

    result->m_parent = this;
    result->m_size = realsize;

    return result;
}

Entity* Scene::entity_at(size_t offset) const
{
    _Arena* curr = m_arenabuff;
    while (offset > _Arena::SIZE)
    {
        offset -= _Arena::SIZE;
        curr = curr->next;
    }

    assert(curr != NULL);

    return reinterpret_cast<Entity*>(curr->data + offset);
}

void Scene::call_update()
{
    { // Update phase region
        update();
        for (auto& ent : *this)
            ent.update();
        //camera.Update(engine.last_dt);
    }

    { // pUpdate phase region
        pupdate();
        for (auto& ent : *this)
            ent.pupdate();
        //camera.pUpdate(engine.phys_target_dt);
    }
}