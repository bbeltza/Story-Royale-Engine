#pragma once
#include <standard>

#include "GameInstance.hpp"
#include "Camera.hpp"

#include "datatypes/color.hpp"
#include "datatypes/vector.hpp"

#include "internal_def.hh"

#include "Base/object.hpp"

__def_internal(__display_render);
__def_internal(__update_world);
__def_internal(__query_objects);
__def_internal(__clean_containers);

namespace sreECS
{
    struct Camera;

    struct Scene: public sre::Object
    {
        Scene();
        ~Scene();

    public:
        /// The background color that the game will have when there's a world, the alpha component is not used
        sre::col4 background{255, 149, 236};
        /// The color that the game will render on top of the world.
        /// This also acts as the background of the GUI Layer
        sre::col4 foreground{sre::col4::INVISIBLE};
        // The camera, whence its coordinates point to the center of the screen
        Game::Camera camera;
    public:
        // Make the scene current, this is the equivalent of calling: `sreECS::Scene::make_current(this)`
        void make_current(bool destroy_old=true) { return make_current(this, destroy_old); }
        // Make a scene current
        // @param scene the scene to be current. Can be `nullptr` if you want no scene to be current
        // @param destroy_old destroy and free, if there is, the old scene. Note that this is by default `true` in the non-static `make_current()` method
        static void make_current(Scene* scene, bool destroy_old);

        // Get the current scene, with a template in which `T` is a type inherited by Scene
        // @returns a pointer to a current scene, or `nullptr` if there's no current scene, or if the current scene isn't or doesn't inherit from `T`
        template <typename T=Scene> static inline T* current() { return current()->cast<T>(); /* Scary ! Note that if current() is nullptr then dynamic_cast will just return nullptr */ }

        // Get the current scene, without templates
        static Scene* current();

        // Calls dynamic_cast to it. It's also fine to just use dynamic_cast
        template <typename T> inline T* cast() const { return dynamic_cast<T*>(this); }
    };
}

namespace Game
{
    class Entity;

    class World : public WorldInstance
    {
    protected:
        World();
        ~World();
    public:
        /// The background color that the game will have when there's a world, the alpha component is not used
        sre::col4 Background = {255, 149, 236};

    public:
        /// The color that the game will render on top of the world.
        /// This also acts as the background of the GUI Layer
        sre::col4 Foreground = sre::col4::INVISIBLE;

        template <class _wType = World, class... _args>
        static inline _wType *setCurrent(_args... args)
        {
            auto new_current = new _wType(args...);
            set(new_current);
            return new_current;
        }

        // Member

        Camera CurrentCamera;

    public:
        // Entity Handling

        // Adds an entity to the world, templated by a derived class of your choice.
        // But make sure it inherits correctly from the class Entity because the engine will have to call Update() on them
        template <class _entity = Entity, class... _args>
        inline _entity *addEntity(_args... args)
        {
            s_TargetWorld = this;
            auto newEntity = new _entity(args...);
            s_TargetWorld = nullptr;

            m_Entities.push_front(newEntity);
            return newEntity;
        }
        // Gets the list of entities that the world has, templated by a derived class of your choice.
        template <class _entity = Entity>
        inline const std::list<_entity *> &getEntities() const { return *(std::list<_entity *> *)(&m_Entities); }

        // Coordinate converting

        // @brief Converts screen coordinates to this world's coordinates
        inline sre::vec2ut screenToWorldSpace(const sre::unit x, const sre::unit y) const { return screenToWorld(x, y, &CurrentCamera); }
        // @brief Converts this world's coordinates into screen coordinates
        inline sre::vec2ut worldToScreenSpace(const sre::unit x, const sre::unit y) const { return worldToScreen(x, y, &CurrentCamera); }

        // Static functions that convert coordinates, without the need of a world

        static sre::vec2ut worldToScreen(const sre::unit x, const sre::unit y, const Camera *cam = nullptr);
        static sre::vec2ut screenToWorld(const sre::unit x, const sre::unit y, const Camera *cam = nullptr);

        // Get the current world's camera, or NULL if there's no world
        static Camera *currentCamera();

        // Gets the current world.
        /// The world that will be updated and rendered.
        template <class _wType = World>
        static _wType *Current() { return dynamic_cast<_wType *>(current()); }
        // Casts the world to a derivate
        template <class _wType>
        _wType *cast() { return dynamic_cast<_wType *>(this); }

    private:
        static void set(World *);
        static World *current();

        Entity *call_query(float *);

        // The container that holds all of its entities
        std::list<Entity *> m_Entities;

        // Static member that tells an entity which world to be in, set to this when world->addEntity() is called
        static World *s_TargetWorld;

        void call_update();
        void call_pupdate();
        void call_render();
        static bool cmp(const Entity *, const Entity *);

        friend class Entity;

        __friend_internal(__display_render);
        __friend_internal(__update_world);
        __friend_internal(__query_objects);
        __friend_internal(__clean_containers);

    public:
        // Iterator stuff (Note that these functions ARE const meaning you cannot change the pointer that the world holds. However, you can still access and modify the entities normally)

        auto begin() const -> decltype(m_Entities.begin())
        {
            return m_Entities.begin();
        }
        auto end() const -> decltype(m_Entities.end()) { return m_Entities.end(); }
    };
}

// This thing is so overcommented ah!!
