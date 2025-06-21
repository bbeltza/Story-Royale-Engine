#pragma once
#include <standard.h>
#include <SDL.hpp>

#include "Window.h"

#include "Flags.h"

#include "Datatypes.h"

#define __push_gui_vec(s_target, vec) {\
s_target = this;\
vec.push_back(new T);\
s_target = nullptr;\
\
return (T*)vec.back(); }

#define __pop_gui_vec(vec) {\
delete vec.back();\
vec.pop_back(); }

namespace Game
{
    class GuiLayer;
    class GuiObject;
    class GuiComponent;
    
    class GuiContainer
    {
    public:
        bool visible = 1;

        GuiContainer();
        virtual ~GuiContainer();

        inline virtual bool isGuiLayer() const = 0;

        virtual void Update(float dt) { }

        void _callUpdate(float dt);

        GuiContainer* getParent() const { return p_parent; }

        inline virtual void _render() { _renderchildren(); };
    protected:
        SDL_FRect p_absolute{ 0, 0, 0, 0 };

        void _processchildren();

        template <typename T>
        inline T* pushGuiObject() __push_gui_vec(s_targetParentContainer, p_children)
        template <typename T>
        inline T* pushGuiComponent() __push_gui_vec(GuiComponent::s_targetComponentParent, p_components)
            
        inline void popGuiObject() __pop_gui_vec(p_children)
        inline void popGuiComponent() __pop_gui_vec(p_components)

        GuiContainer* p_parent;
        std::vector<GuiContainer*> p_children;
        std::vector<GuiComponent*> p_components;
        
    private:
        friend void Engine::Window::processGui();
        friend class GuiObject;
        friend class GuiComponent;

        static GuiContainer* s_targetParentContainer;

        inline void _renderchildren()
        {
            for (GuiContainer* obj : p_children)
            {
                obj->_render();
                obj->_renderchildren();
            }
        }
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

    private:
        friend class GuiContainer;

        void _process();
    };

    class GuiComponent
    {
    public:
        GuiComponent();
        virtual ~GuiComponent() {}

        bool enabled = false;

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

        SDL_FRect* getParentAbs() { return &p_parent->p_absolute; };

    private:
        friend class GuiContainer;
        static GuiContainer* s_targetComponentParent;

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