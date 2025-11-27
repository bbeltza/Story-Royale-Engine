#pragma once
#include <standard>

#include "GameInstance.hpp"
#include "Camera.hpp"

#include "Datatypes/Color.hpp"
#include "Datatypes/Vector.hpp"

#include "internal_def.hh"

class InputClass;
class DrawingDevice;
class EngineClass;

__def_internal(__display_render)
__def_internal(__update_world)
__def_internal(__query_objects)
__def_internal(__clean_containers)

namespace Game
{
    class Entity;

    class World : public WorldInstance
    {
    protected:
        World();
        ~World();
    public:

        // Static

        /// @brief The background color that the game will have when there's a world
        Color3 Background = { 255, 149, 236 };
    private:
        char _padding;
    public:
        /// The color that the game will render on top of the world.
        /// This acts also as the background of the GUI Layer
        Color4 Foreground = Color4::INVISIBLE;

        template <class _wType=World, class... _args>
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
        template <class _entity=Entity, class... _args>
        inline _entity *addEntity(_args... args)
        {
            s_TargetWorld = this;
            auto newEntity = new _entity(args...);
            s_TargetWorld = nullptr;

            Entity* implicit_check = newEntity;
            return newEntity;
        }
        // Gets the list of entities that the world has, templated by a derived class of your choice.
        template <class _entity=Entity> inline const std::list<_entity *> &getEntities() const { return *(std::list<_entity *> *)(&m_Entities); }

        // Coordinate converting

        // @brief Converts screen coordinates to this world's coordinates
        inline Vector2ut screenToWorldSpace(const Unit x, const Unit y) const { return screenToWorld(x, y, &CurrentCamera); }
        // @brief Converts this world's coordinates into screen coordinates
        inline Vector2ut worldToScreenSpace(const Unit x, const Unit y) const { return worldToScreen(x, y, &CurrentCamera); }

        // Static functions that convert coordinates, without the need of a world

        static Vector2ut worldToScreen(const Unit x, const Unit y, const Camera *cam = nullptr);
        static Vector2ut screenToWorld(const Unit x, const Unit y, const Camera *cam = nullptr);

        // Get the current world's camera, or NULL if there's no world
        static Camera* currentCamera();

        // Gets the current world.
        /// The world that will be updated and rendered.
        template <class _wType=World> static _wType* Current() {return dynamic_cast<_wType*>(current());}
        // Casts the world to a derivate
        template <class _wType> _wType* cast() {return dynamic_cast<_wType*>(this);}

    private:
        static void set(World*);
        static World* current();

        Entity *call_query(float*);

        // The container that holds all of its entities
        std::list<Entity *> m_Entities;

        // Static member that tells an entity which world to be in, set to this when world->addEntity() is called
        static World* s_TargetWorld;

        void call_update();
        void call_pupdate();
        void call_render();
        static bool cmp(const Entity*, const Entity*);

        friend class Entity;
        friend class ::InputClass;
        friend class ::DrawingDevice;
        friend class ::EngineClass;

        __friend_internal(__display_render)
        __friend_internal(__update_world)
        __friend_internal(__query_objects)
        __friend_internal(__clean_containers)
    };
}

// This thing is so overcommented ah!!
