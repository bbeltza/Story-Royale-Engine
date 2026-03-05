#include <ECS/Scene.hpp>
#include <ECS/Entity.hpp>
#include <ECS/Component.hpp>

#include <Core/Display.hpp>
#include <Core/Input.hpp>

#include "../internal.h"

#ifdef _CRT_BEGIN_C_HEADER
    #define malloc_aligned(size) _aligned_malloc(size, _Arena::PAGE_SIZE)
    #define free_aligned(block) _aligned_free(block)
#elif _POSIX_C_SOURCE >= 200112L || XOPEN_SOURCE >= 600
    static inline void* _malloc_aligned(size_t size, size_t alignment) // Need a stupid function for posix because posix_memalign doesn't return the pointer but instead it is passed as a parameter
    {
        void* block;
        if (posix_memalign(&block, alignment, size) != 0)
            return NULL;

        return block;
    }
    #define malloc_aligned(size) _malloc_aligned(size, _Arena::PAGE_SIZE)
    #define free_aligned(block) free(block)
#else
    #define malloc_aligned(size) operator new(size)
    #define free_aligned(block) operator delete(block)
#endif

sre::vec2ut sreECS::mouse_worldcoords() { return Scene::current()->camera.toWorldSpace(sre::mouse_screencoords()); }

using namespace sreECS;

const Entity* Scene::s_querying = NULL;

Scene::_Arena* Scene::new_arena()
{
    _Arena* buff = static_cast<_Arena*>(malloc_aligned(sizeof(*buff)));
    buff->next = NULL;

    return buff;
}

Scene::Scene(): m_arenabuff(new_arena()), m_entity_end(reinterpret_cast<Entity*>(m_arenabuff->data))
{
}

Scene::~Scene()
{
    for (Entity& ent : *this)
        ent.~Entity();

    assert(m_arenabuff != NULL);
    do
    {
        _Arena* next = m_arenabuff->next;
        free_aligned(m_arenabuff);
        m_arenabuff = next;
    } while (m_arenabuff);
    
    if (engine.current_world == this)
        engine.current_world = NULL;
}

void Scene::make_current(Scene* scene, bool destroy_old)
{
    if (engine.current_world && destroy_old)
        static_cast<Scene*>(engine.current_world)->destroy();
    
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

    // TODO: add code that detects if `size` is larger than the arena page size and if so just mallocate outsize the arena
    if (realsize > _Arena::SIZE)
    {
        result = static_cast<Entity*>(operator new(realsize));
        goto ENTITY_SETUP;
    }
    //

    for (auto& ent : m_freelist)
    {
        if (ent->m_size < realsize)
            continue;

        result = ent;
        if (ent->m_size == realsize)
        {
            ent = m_freelist.back();
            m_freelist.pop_back();
        }
        else
        {
            size_t newsize = ent->m_size - realsize;

            ent += realsize;
            ent->m_size = newsize;
        }
    }

    if (result) goto ENTITY_SETUP;

    if (m_entities.empty())
    {
        result = reinterpret_cast<Entity*>(m_arenabuff->data);
        m_entity_end = reinterpret_cast<Entity*>(m_arenabuff->data + realsize);
    }
    else
    {
        result = m_entity_end;
        sre::sptr remaining_diff = (reinterpret_cast<sre::byte*>(result) + realsize) - (m_arenabuff->data + m_arenabuff->SIZE);
        if (remaining_diff > 0)
        {
            // Append remaining chunk into the freelist in case of it being used
            m_entity_end->m_size = remaining_diff;
            m_freelist.push_back(m_entity_end);
            //

            _Arena* arena = new_arena();
            arena->next = m_arenabuff;
            m_arenabuff = arena;
            m_entity_end = reinterpret_cast<Entity*>(arena->data);
            result = m_entity_end;
        }
        m_entity_end = reinterpret_cast<Entity*>(reinterpret_cast<sre::byte*>(m_entity_end) + realsize);
    }
    
    ENTITY_SETUP:
    assert(result != NULL);

    m_entities.push_back(result);

    if (_realsize) *_realsize = realsize;

    result->m_parent = this;
    result->m_size = realsize;

    return result;
}

void Scene::call_update()
{
    { // Update phase region
        update();
        for (auto& ent : *this)
        {
            ent.update();
            for (auto& comp : ent)
            {
                if (comp.enabled())
                    comp.on_update(ent);
            }
            ent.updated.fire();
        }
        camera.update();
    }

    static sre::timeStamp dt_accumulated;
    dt_accumulated += engine.last_dt;
    if (dt_accumulated < 0)
        camera.clamp_position(); // At least clamp the camera to the border whenever there are no updates
    else do { // pUpdate phase region
        dt_accumulated -= engine.phys_target_dt;

        pupdate();
        for (auto& ent : *this)
        {
            const_cast<sre::vec2ut&>(ent.lastVelocity) = ent.position;

            ent.pupdate();
            for (auto& comp : ent)
            {
                if (comp.enabled())
                    comp.on_pupdate(ent);
            }

            const_cast<sre::vec2ut&>(ent.lastVelocity) = ent.position - ent.lastVelocity;
        }
        camera.pupdate();
    }  while (dt_accumulated > 0);

    updated.fire();
}

void Scene::call_render()
{
    // Sort entities by z_index. Cannot use default std::sort because of the need to access the entity buffer with entity_at()
    // So I made my own sorting algorithm :r) (It's literally just gnome sort ahahah)
    for (auto i2 = m_entities.begin();;)
    {
        auto i1 = i2++;
        if (i2 == m_entities.end())
            break;

        Entity*& entity1 = *i1;
        Entity*& entity2 = *i2;

        if (entity1->z_index > entity2->z_index)
        {
            std::swap(entity1, entity2); // Might still keep up the xor swapping!!
            i2--;
        }
    }

    pre_render();

    for (auto& ent : *this)
    {
        ent.pre_render();
        for (auto& comp : ent)
        {
            if (comp.enabled())
                comp.on_render(ent);
        }
        ent.post_render();
    }

    post_render();

    rendered.fire();
}

bool Scene::call_query(sre::vec2ut coords) const
{
    s_querying = NULL;

    for (auto it = rbegin(); it != rend(); ++it)
    {
        auto& entity = *it;
        for (auto& comp : entity)
        {
            if (comp.on_query(entity, coords))
            {
                s_querying = &entity;
                return true;
            }
        }
    }

    return false;
}
