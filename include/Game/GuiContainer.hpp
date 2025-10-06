#pragma once
#include <standard>
#include "Game/GameInstance.hpp"

#include "Datatypes/Color.hpp"
#include "Datatypes/Rect.hpp"
#include "Datatypes/TimeStamp.h"

class DrawingDevice;
class InputClass;
class EngineClass;

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

        inline GuiContainer *getParent() const { return m_parent; }
        inline const std::list<GuiObject*>& getChildren() const { return m_children; }

        inline Vector2f getAbsolutePosition() const { return m_absolute.Position; }
        inline Vector2f getAbsoluteSize() const { return m_absolute.Size; }

        template <class obj_type, class... args> inline obj_type* addChild(args... arg)
        {
            s_targetParentContainer = this;
            auto obj = new obj_type(arg...);
            s_targetParentContainer = nullptr;
            m_children.push_back(reinterpret_cast<GuiObject*>(obj));
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

        GuiObject* _query();

        GuiContainer *m_parent = nullptr;
        std::list<GuiObject*> m_children;
        std::list<GuiComponent*> m_components;
        
        Color4 m_modulate = {255, 255, 255, 255};
        RectF m_absolute = {0, 0, 0, 0};

        static GuiContainer* s_targetParentContainer;

        friend class ::DrawingDevice;
        friend class ::InputClass;
        friend class ::EngineClass;
        friend class GuiObject;
        friend class GuiComponent;
    };
}