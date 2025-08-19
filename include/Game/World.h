#pragma once
#include "standard.h"

#include "GameInstance.h"
#include "Camera.h"

#include "Datatypes/Color.h"
#include "Datatypes/Vector.h"

class InputClass;
class DrawingDevice;
class EngineClass;

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
        static Color3 Background;
        /// The color that the game will render on top of the world.
        /// This acts also as the background of the GUI Layer
        static Color4 Foreground;

        template <class w_type>
        static inline w_type *setCurrent()
        {
            if (m_Current)
                delete m_Current;

            auto new_current = new w_type;
            if (checkCurrent(reinterpret_cast<World *>(new_current)))
                return new_current;
            delete new_current;
            err();
            
            return nullptr;
        }

        // Member

        Camera CurrentCamera;

    public:
        // Entity Handling

        // Adds an entity to the world, templated by a derived class of your choice.
        // But make sure it inherits correctly from the class Entity because the engine will have to call Update() on them
        template <class _entity>
        inline _entity *addEntity()
        {
            s_TargetWorld = this;
            auto newEntity = new _entity;
            s_TargetWorld = nullptr;
            return newEntity;
        }
        // Adds a Base Entity to the world, it's just an alias for addEntity<Entity>();
        Entity *addEntity();
        // Gets the list of entities that the world has, templated by a derived class of your choice.
        template <class _entity>
        inline const std::list<_entity *> &getEntities() const
        {
            return *(std::list<_entity *> *)(&m_Entities);
        }

        // Coordinate converting

        // @brief Converts screen coordinates to this world's coordinates
        inline Vector2f screenToWorldSpace(const float x, const float y) { return screenToWorld(x, y, &CurrentCamera); }
        // @brief Converts this world's coordinates into screen coordinates
        inline Vector2f worldToScreenSpace(const float x, const float y) { return worldToScreen(x, y, &CurrentCamera); }

        // Static functions that convert coordinates, without the need of a world

        static Vector2f worldToScreen(const float x, const float y, Camera *cam = nullptr);
        static Vector2f screenToWorld(const float x, const float y, Camera *cam = nullptr);

        // Gets the current world.
        /// The world that will be updated and rendered.
        template <class _wType> static _wType* Current() {return reinterpret_cast<_wType>(m_Current);}
        // Casts the world to a derivate
        /// Use it with precaution
        template <class _wType> _wType* cast() const {return reinterpret_cast<_wType>(this);}

        // Override methods

        void Update(TimeStamp) override;
        void pUpdate(TimeStamp) override;

    private:
        Entity *_query();

        // Used to check whether a world is really a world, it points at a static variable in World.cpp
        const char* const m_worldTag;

        static World* checkCurrent(World*);
        static void err();

        // The container that holds all of its entities
        std::list<Entity *> m_Entities;

        static Vector2f center;
        // Static member that tells an entity which world to be in, set to this when world->addEntity() is called
        static World* s_TargetWorld;

        static void call_render();
        static bool cmp(const Entity*, const Entity*);

        friend class Entity;
        friend class ::InputClass;
        friend class ::DrawingDevice;
        friend class ::EngineClass;

        static World *m_Current;
    };

    // The method to be defined by the game using the engine
    // EVERY game should have this defined, even if it's empty
    // This is the entry point for the game, it's called after initializing the libraries, but before creating the window
    // You can change the game settings here (Untill a json implementation is goind to be added)
    void Initialize();
}

// This thing is so overcommented ah!!