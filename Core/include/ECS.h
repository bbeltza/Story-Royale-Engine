#pragma once
#include <vector>

#include "Color.h"

enum ParentType : char
{
    WORLD,
    COMPONENT
};
enum ScaleType : char
{
    XY,
    XX
};
enum ComponentType : char
{
    SHAPE,
    ENTITY_CONTAINER
};

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

    class Camera
    {
        float x = 0, y = 0;
        ScaleType scalingType = XY;

        union
        {
            double Scale = 1;
            struct
            {
                float ScaleX, ScaleY;
            };
        };

    };

    class Entity : public GameInstance
    {
    public:
        float x = 0, y = 0;
        
        Entity();
        virtual ~Entity();

        void popComponent();
        void removeComponent(unsigned int index);
        void removeComponent(void* address);


    private:

        ParentType m_ParentType = WORLD;
        union
        {
            World* m_world;
            Component* m_cParent;
        };

        std::vector<Component*> m_Components;

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
        void popEntity();
        void removeEntity(unsigned int index);
        void removeEntity(void* address);

        void Update(float dt);
        void Render();
        virtual void OnUpdate(float dt) {}

    private:
        static World* s_TargetEntityWorld;
        std::vector<Entity*> m_Entities;
        friend Entity::Entity();
    };

    class Component : public GameInstance
    {
        Component(ComponentType type);
    private:
        const ComponentType m_type;
        Entity* parent;
    };

    extern World* currentWorld;

    }

class EntryWorld : public Game::World
{
public:
    EntryWorld();
};

#define Game_SetWorld(T) { if (Game::currentWorld) delete Game::currentWorld; Game::currentWorld = new T; }