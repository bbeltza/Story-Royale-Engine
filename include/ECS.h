#pragma once
#include <standard.h>

#include "Datatypes.h"

#include "Enums.h"
#include "Flags.h"

#define pushComponentPtr(type, ptr) pushComponent(type, (void**)&ptr)

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
        int zIndex = 0;

        Entity();
        virtual ~Entity();


        World* getWorld() const { return m_world; }

        Components::Velocity* velocityComp = nullptr;

        void* pushComponent(ENUM_ComponentType type, void **ptr=nullptr);
        void popComponent();

    private:
        friend class World;
        friend class Component;

        ENUM_ParentType m_ParentType = WORLD;
        union
        {
            World* m_world;
            Component* m_cParent;
        };

        std::list<Component*> m_Components;

        void _debugDraw();
        void _render();
        inline void _pProcess(float dt);

        static Entity* s_targetEntityComponent;
        static unsigned char s_targetComponentType;

    };

    class World: public GameInstance
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
        void popEntity() { delete m_Entities.back(); }
        
        inline Vector2f screenToWorldSpace(const int x, const int y) {return screenToWorld(x, y, &CurrentCamera);}
        inline Vector2i worldToScreenSpace(const float x, const float y) {return worldToScreen(x, y, &CurrentCamera);}

        static Vector2i worldToScreen(const float x, const float y, Camera* cam=nullptr);
        static Vector2f screenToWorld(const int x, const int y, Camera* cam=nullptr);

        void Update(float dt);
        void pUpdate(float dt);
        static void render();
        virtual void OnUpdate(float dt) {}

        Entity* _query();

    private:
        static int center[2];
        static World* s_TargetEntityWorld;
        std::list<Entity*> m_Entities;

        friend class Entity;

        static bool re_order(const Entity* first, const Entity* second) { return first->zIndex < second->zIndex; }
    };

    extern World* currentWorld;

    template <typename T>
    inline World* setWorld()
    {
        if (currentWorld) currentWorld->Destroy();
        currentWorld = new T;
        return currentWorld;
    }

    void Initialize();
}