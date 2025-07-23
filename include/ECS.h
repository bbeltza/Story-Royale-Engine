#pragma once
#include <standard.h>

#include "Datatypes.h"

#include "Enums.h"
#include "Flags.h"
#include "Engine.h"

#define pushComponentPtr(type, ptr) pushComponent(type, (void **)&ptr)

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
        RectF clamp = {0, 0, 0, 0};
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
        enum ProcessFlags : uint8_t
        {
            p_Special = 0,
            p_Render = (1 << 0),
            p_Update = (1 << 1),
            p_pUpdate = (1 << 2),
            p_Query = (1 << 3)
        };

    public:
        Component();
        virtual ~Component();
        virtual inline void Render(Entity *_entity) {}
        virtual inline void Update(Entity *_entity, float delta) {}
        virtual inline void pUpdate(Entity *_entity, float delta) {}
        virtual inline bool Query(Entity *_entity) { return false; }

        inline const ProcessFlags getProcessFlags() const { return p_flags; }

    protected:
        ProcessFlags p_flags;
    private:
    friend class ::EngineClass;
    static void ClearComponents();
    };

    class Entity : public GameInstance
    {
    public:
        float x = 0, y = 0;
        int zIndex = 0;

        Entity();
        virtual ~Entity();

        inline World *getWorld() const { return m_world; }
        template <class w_Type>
        inline World *getWorld() const { return (w_Type *)m_world; }

        Components::Velocity *velocityComp = nullptr;

        template <class comp_Type>
        comp_Type *addComponent()
        {
            comp_Type *newComp = new comp_Type;
            m_Components.push_back(newComp);
            return newComp;
        }

        Game::Components::Velocity* addVelocityComponent();
        
        void addComponent(void *_component) { m_Components.push_back(reinterpret_cast<Component *>(_component)); }

        void popComponent();

        void reParent(World* const _world);

    private:
        friend class World;
        friend class Component;

        ENUM_ParentType m_ParentType = WORLD;
        union
        {
            World *m_world;
            Component *m_cParent;
        };

        std::list<Component *> m_Components;

        void _debugDraw();
        void _render();
        inline void _pProcess(float dt);
    };

    class World : public GameInstance
    {
    public:
        World();
        ~World();
        static Color3 Background;
        static Color4 Foreground;

        Camera CurrentCamera;

        template <class T>
        inline T *addEntity()
        {
            this->s_TargetEntityWorld = this;
            T *newEntity = new T;
            this->s_TargetEntityWorld = nullptr;
            return newEntity;
        }
        template <class T>
        inline T *addEntity(float x, float y)
        {
            this->s_TargetEntityWorld = this;
            T *newEntity = new T;
            newEntity->x = x;
            newEntity->y = y;
            this->s_TargetEntityWorld = nullptr;
            return newEntity;
        }
        void popEntity() { delete m_Entities.back(); }

        inline Vector2f screenToWorldSpace(const float x, const float y) { return screenToWorld(x, y, &CurrentCamera); }
        inline Vector2f worldToScreenSpace(const float x, const float y) { return worldToScreen(x, y, &CurrentCamera); }

        static Vector2f worldToScreen(const float x, const float y, Camera *cam = nullptr);
        static Vector2f screenToWorld(const float x, const float y, Camera *cam = nullptr);

        void Update(float dt);
        void pUpdate(float dt);
        static void render();
        virtual void OnUpdate(float dt) {}

        Entity *_query();

    private:
        static int center[2];
        static World *s_TargetEntityWorld;
        std::list<Entity *> m_Entities;

        friend class Entity;

        static bool re_order(const Entity *first, const Entity *second) { return first->zIndex < second->zIndex; }
    };

    extern World *currentWorld;

    template <typename T>
    inline World *setWorld()
    {
        if (currentWorld)
            currentWorld->Destroy();
        currentWorld = new T;
        return currentWorld;
    }

    void Initialize();
}