#ifndef _SREECS_SCENE_HPP
#define _SREECS_SCENE_HPP

// This is all for testing since I wanna get my game's concepts done first before doing anything else here
#include <Datatypes/Vector.hpp>
#include <Core/Texture.hpp>
namespace sre
{
    // Lmao should I add this?
    template <typename T>
    class Animation;
}
namespace sr
{
    
    struct Character
    {
        sre::vec2ut position;
        sre::Animation<sre::Texture> sprites;
    };
}
//

namespace _sreECS
{
    class System;
    struct Scene;

    class System
    {
        Scene* const m_scene;
        void* const m_entity;

        protected:
        virtual void update() = 0;
    };

    struct Scene
    {
        template <typename Entity>
        Entity& add_entity();

        template <typename Entity>
        System& add_system(Entity& entity);
    };
}

#endif