#pragma once
#include <standard.h>
#include <SDL.hpp>

#include "Window.h"

#include "Flags.h"

#include "Datatypes.h"

#define __push_gui_list(s_target, list, target_type) {\
s_target = this;\
list.push_front((target_type*)(new T));\
s_target = nullptr;\
\
return (T*)list.front(); }

namespace Game
{
    class GuiLayer;
    class GuiObject;
    class GuiComponent;

    class GuiContainer: public GameInstance
    {
        friend class GuiObject;
        friend class GuiComponent;
    public:
        bool visible = 1;

        GuiContainer();
        virtual ~GuiContainer();

        inline virtual bool isGuiLayer() const = 0;

        //void Update(float dt) override { }

        void _callUpdate(float dt);

        GuiContainer* getParent() const { return p_parent; }

        inline virtual void _render() { _renderchildren(); };

        GuiObject* _query();
        bool canQuery = true;

    protected:
        friend class ::WindowClass;

        Color4 p_modulate = { 255, 255, 255, 255 };
        SDL_FRect p_absolute{ 0, 0, 0, 0 };

        void _processchildren();

        template <typename T>
        inline T* pushGuiObject() __push_gui_list(s_targetParentContainer, p_children, GuiContainer)
        template <typename T>
        inline T* pushGuiComponent() __push_gui_list(s_targetComponentParent, p_components, GuiComponent)
            
        inline void popGuiObject() { delete p_children.front(); }
        void popGuiComponent();

        GuiContainer* p_parent = nullptr;
        std::list<GuiContainer*> p_children;
        std::list<GuiComponent*> p_components;
        
    private:
        static GuiContainer* s_targetParentContainer;
        static GuiContainer* s_targetComponentParent;

        inline void _renderchildren()
        {
            _prerender_components();
            _render_components();

            for (GuiContainer* obj : p_children)
            {
                if (!obj->visible) continue;

                obj->_prerender_components();
                obj->_render();
                obj->_render_components();
                obj->_renderchildren();
            }
        }

        void _prerender_components();
        void _render_components();
        void _procpos_components();
        void _procsize_components();
        void _proc_children_components();
        
    };

    class GuiComponent
    {
    public:
        GuiComponent();
        virtual ~GuiComponent();

        bool enabled = true;

        inline const FLAG_GuiComponentUpdateFlags getUpdateFlags() const { return (FLAG_GuiComponentUpdateFlags)p_flags; }
        inline bool hasFlag(FLAG_GuiComponentUpdateFlags flag) const
        {
            return p_flags & flag;
        }

        virtual void render() {};
        virtual void pre_render() {};
        virtual void process_size() {};
        virtual void process_position() {};
        virtual void process_children(GuiObject* Object) {};

    protected:
        unsigned char p_flags = 0;
        GuiContainer* p_parent;

        SDL_FRect* getParentAbs() { return &p_parent->p_absolute; }
        Color4* getParentMod() { return &p_parent->p_modulate; }

    private:
        friend class GuiContainer;

    };

    class GuiLayer: public GuiContainer
    {
    public:
        Color4 Foreground{0, 0, 0, 0};

        GuiLayer() {}
        ~GuiLayer() {}

        //void _callUpdate(float dt) override;

        bool isGuiLayer() const override { return 1; }

    };

    class GuiObject : public GuiContainer
    {
    public:
        GuiObject():position(0, 0, 0, 0), size(0, 300, 0, 300), anchor(0, 0) {}

        UDim2 position;
        UDim2 size;
        Vector2f anchor;
        Color4 color{255, 255, 255, 255};
        
        void _render() override;
        bool isGuiLayer() const override { return 0; }
        bool isHovering() const;

    private:
        friend class GuiContainer;

        void _process();
    };

    extern GuiLayer* currentGuiLayer;

    template <typename T>
    inline GuiLayer* setGuiLayer()
    {
        if (currentGuiLayer) delete currentGuiLayer;
        currentGuiLayer = new T;
        return currentGuiLayer;
    }
}

class EntryGuiLayer : public Game::GuiLayer
{
public:
    EntryGuiLayer();
    ~EntryGuiLayer();

    void Update(float dt) override;
};

#undef __push_gui_vec
#undef __pop_gui_vec