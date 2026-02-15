#ifndef _SREECS_SCENE_HPP
#define _SREECS_SCENE_HPP

namespace _sreECS
{
    class Entity
    {
        size_t m_size;

    public:

    };

    class System
    {
    public:
        virtual void update(Entity* entity) {}
        virtual void p_update(Entity* entity) {}
        virtual void pre_render(Entity* entity) {}
        virtual void post_render(Entity* entity) {}
        virtual void render(Entity* entity) {}
    };

    struct Scene
    {
        
    };
}

#endif