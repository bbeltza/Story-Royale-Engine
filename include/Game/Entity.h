#pragma once
#include "standard.h"

#include "GameInstance.h"

#include "Datatypes/Vector.h"

namespace Game
{
    class World;
    class Component;

    class Entity:public WorldInstance
    {
        protected:
        Entity();
        ~Entity();
        public:
        
        enum ParentType: uint8_t
        {
            WorldParent,
            ComponentParent
        };

        public:
        Vector2f Position;
        int zIndex = 0;

        template <class w_Type> inline w_Type* getWorld() const { return reinterpret_cast<w_Type*>(m_world); }
        inline void addComponent(Component* const component) { m_Components.push_back(component); }
        inline void enableComponent(Component* const component) { m_DisabledComps.erase(component); }
        inline void disableComponent(Component* const component) { m_DisabledComps.insert(component); }
        inline bool componentDisabled(Component* const component) {return m_DisabledComps.find(component) != m_DisabledComps.end();}

        void reParent(World* const _world);

        private:
        std::list<Component*> m_Components;
        std::unordered_set<Component*> m_DisabledComps;

        // Draws a red cross representing the entity's position, only calls in debug mode
        void _debugDraw();
        // Calls Render() to all of its components
        // Entities by default don't render itself, the components with the Render process flag do
        void call_render();
        void call_pupdate(TimeStamp);

        ParentType m_ParentType = WorldParent;
        union {
            // The parent world of the entity
            World* m_world;
            // The parent component of the entity if it has been added by an EntityContainer component
            // EntityContainer components allow entities to store another entities, they aren't done yet so it's unused
            Component *m_cParent;
        };

        friend class World;
    };
}