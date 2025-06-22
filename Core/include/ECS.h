#pragma once
#include <standard.h>

#include "Datatypes.h"

#include "Enums.h"
#include "Flags.h"

#define pushComponentPtr(type, ptr) pushComponent(type, (void**)&ptr)

struct GameInstance
{
    virtual void Update(float dt) {}
    virtual void pUpdate(float dt) {}
    virtual void preRender() {}
    virtual void postRender() {}
};


namespace Game
{
    class World;
    class Entity;
    class Component;

    namespace Components
    {
        class Velocity;
    }

    class Camera
    {
    public:
        float x = 0, y = 0;
        ENUM_ScaleType scalingType = XY;

        union
        {
            double Scale = 1;
            struct
            {
                float ScaleX, ScaleY;
            };
        };

    };

    class Component
    {
    public:
        Component();
        virtual ~Component();
        virtual inline void render(int x, int y) {};

        inline Entity* getParent() const { return m_parent; }
        inline ENUM_ComponentType getType() const { return m_type; }

    private:

        const ENUM_ComponentType m_type;
        Entity* const m_parent;
    };

    class Entity : public GameInstance
    {
    public:
        float x = 0, y = 0;

        Entity();
        virtual ~Entity();


        World* getWorld() const { return m_world; }

        Components::Velocity* velocityComp = nullptr;


    protected:

        void* pushComponent(ENUM_ComponentType type, void **ptr=nullptr);
        void popComponent();
        void removeComponent(unsigned int index);
        void removeComponent(void* address);

    private:
        friend class World;
        friend Component::Component();
        ENUM_ParentType m_ParentType = WORLD;
        union
        {
            World* m_world;
            Component* m_cParent;
        };

        std::vector<Component*> m_Components;

        void _debugDraw();
        void _render();
        inline void _pProcess(float dt);

        static Entity* s_targetEntityComponent;
        static unsigned char s_targetComponentType;

    };

    class World
    {
    public:
        World();
        ~World();
        Color3 Background = { 255, 149, 236 };
        Color4 Foreground = { 0 };

        Camera CurrentCamera;

        template <class T>
        inline T* addEntity()
        {
            this->s_TargetEntityWorld = this;
            T* newEntity = new T;
            this->s_TargetEntityWorld = nullptr;
            return newEntity;
        }
        template <class T>
        inline T* addEntity(float x, float y)
        {
            this->s_TargetEntityWorld = this;
            T* newEntity = new T;
            newEntity->x = x;
            newEntity->y = y;
            this->s_TargetEntityWorld = nullptr;
            return newEntity;
        }
        void popEntity();
        void removeEntity(unsigned int index);
        void removeEntity(void* address);

        Vector2f screenToWorldSpace(int x, int y);
        Vector2i worldToScreenSpace(float x, float y);

        void Update(float dt);
        void pUpdate(float dt);
        void render();
        virtual void OnUpdate(float dt) {}

        Entity* _query();

    private:
        static unsigned int center[2];
        static World* s_TargetEntityWorld;
        std::vector<Entity*> m_Entities;

        friend class Entity;
    };

    extern World* currentWorld;

    template <typename T>
    inline World* setWorld()
    {
        if (currentWorld) delete currentWorld;
        currentWorld = new T;
        return currentWorld;
    }
}

class EntryWorld : public Game::World
{
public:
    EntryWorld();
};