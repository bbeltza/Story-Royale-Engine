#pragma once
#include <standard>
#include "Game/GameInstance.hpp"

#include "datatypes/Color.hpp"
#include "datatypes/rect.hpp"
#include "datatypes/timestamp.h"

#include "internal_def.hh"

class DrawingDevice;
class InputClass;
class EngineClass;

__def_internal(__update_layer)
__def_internal(__display_render)
__def_internal(__query_objects)
__def_internal(__clean_containers)

namespace Game
{
    class GuiComponent;
    class GuiObject;

    class GuiContainer : public GameInstance
    {
    public:
        GuiContainer();
        virtual ~GuiContainer();
        virtual bool isGuiLayer() const = 0;

        template <class _container=GuiContainer>
        inline _container *getParent() const { return m_parent->cast<_container>(); }
        inline const std::list<GuiObject*>& getChildren() const { return m_children; }

        template <class _container=GuiContainer>
        inline _container* cast() { return dynamic_cast<_container*>(this); }

        inline sre::vec2ut getAbsolutePosition() const { return m_absolute.position; }
        inline sre::vec2ut getAbsoluteSize() const { return m_absolute.size; }

        template <class _obj=GuiObject, class... args>
        inline _obj* addChild(args&&... arg)
        {
            s_targetParentContainer = this;
            auto obj = new _obj(std::forward<args>(arg)...);
            s_targetParentContainer = nullptr;
            
            m_children.push_back(obj);
            return obj;
        }

        inline void addComponent(GuiComponent& _component) {m_components.push_back(&_component);}
        inline void removeComponent(GuiComponent& _component) {m_components.remove(&_component);}

        bool visible = true, canQuery = true;
    private:
        void call_update(TimeStamp dt);
        
        void _processchildren();
        void _renderchildren();
        void _prerender_components();
        void _render_components();
        void _procpos_components();
        void _procsize_components();
        void _proc_children_components(uint32_t index);

        GuiObject* _query(float* pt);

        GuiContainer *m_parent = nullptr;
        std::list<GuiObject*> m_children;
        std::list<GuiComponent*> m_components;
        
        Color4 m_modulate = {255, 255, 255, 255};
        sre::rect2Dut m_absolute = {0, 0, 0, 0};

        static GuiContainer* s_targetParentContainer;

        friend class GuiObject;
        friend class GuiComponent;

        __friend_internal(__display_render)
        __friend_internal(__update_layer)
        __friend_internal(__query_objects)
        __friend_internal(__clean_containers)
    };
}