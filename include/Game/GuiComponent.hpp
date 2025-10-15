#pragma once
#include "Game/GuiContainer.hpp"
#include "Game/GuiObject.hpp"

#include "Datatypes/Flags.hpp"

namespace Game
{
    class GuiComponent
    {
    public:
        virtual ~GuiComponent() {}

        bool enabled = true;
    protected:
        virtual void render(GuiContainer*) {}
        virtual void pre_render(GuiContainer*) {}
        virtual void process_size(GuiContainer*) {}
        virtual void process_position(GuiContainer*) {}
        virtual void process_children(GuiObject* Object, uint32_t index) {}

        inline RectF* getAbsolute(GuiContainer* container) { return &container->m_absolute; }
        inline Color4* getModulate(GuiContainer* container) { return &container->m_modulate; }
        inline RectF* getAbsolute(GuiObject* container) { return &container->m_absolute; }
        inline Color4* getModulate(GuiObject* container) { return &container->m_modulate; }
    private:
        friend class GuiContainer;
    };
}