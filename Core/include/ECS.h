#pragma once
#include <vector>

#include "Color.h"
#include "Window.h"

enum ENUM_ParentType : char
{
    WORLD,
    COMPONENT
};
enum ENUM_ScaleType : char
{
    XY,
    XX
};
enum ENUM_ComponentType : char
{
    SHAPE,
    ENTITY_CONTAINER
};

enum ENUM_Shape : char
{
    RECTANGLE,
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
        Component(ENUM_ComponentType type);
        inline void render(int x, int y);

        union
        {
            struct
            {
                int xOffset, yOffset, Width, Height;
                Color4 Color;
                char Shape;
                char Flags;
            } ShapeComponent;
            struct
            {

            } EntityContainerComponent;
        };
    private:

        void initComponent();
        const ENUM_ComponentType m_type;
        Entity* parent;
    };

    class Entity : public GameInstance
    {
    public:
        float x = 0, y = 0;

        Entity();
        virtual ~Entity();

        Component* pushComponent(ENUM_ComponentType type);
        void popComponent();
        void removeComponent(unsigned int index);
        void removeComponent(void* address);

        World* getWorld() { return m_world; }

        inline void render();

    private:
        friend Component::Component(ENUM_ComponentType type);
        ENUM_ParentType m_ParentType = WORLD;
        union
        {
            World* m_world;
            Component* m_cParent;
        };

        std::vector<Component*> m_Components;
        static Entity* s_targetEntityComponent;

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

        struct v2 { int x, y; };
        struct v2 screenToWorldSpace(int x, int y);
        struct v2 worldToScreenSpace(float x, float y);

        void Update(float dt);
        void render();
        virtual void OnUpdate(float dt) {}

    private:
        static unsigned int center[2];
        static World* s_TargetEntityWorld;
        std::vector<Entity*> m_Entities;
        friend Entity::Entity();
        friend void Entity::render();
    };

    extern World* currentWorld;

}

class EntryWorld : public Game::World
{
public:
    EntryWorld();
};

#define Game_SetWorld(T) { if (Game::currentWorld) delete Game::currentWorld; Game::currentWorld = new T; }