#pragma once
#include "Game/GuiContainer.hpp"
#include "Game/GuiObject.hpp"

#include "Datatypes/Flags.hpp"

namespace Game
{
    class GuiComponent
    {
    public:
        enum UpdateFlags: Flags8::Type
        {
            NOUPDATEFLAGS = 0,

            PROCESS_SIZE = bit(0),
            PROCESS_POSITION = bit(1),
            PROCESS_CHILDREN = bit(2),
            RENDER = bit(3),
            PRE_RENDER = bit(4),

            CUSTOMUPDATEFLAGS = NOUPDATEFLAGS
        };
    public:
        GuiComponent();
        virtual ~GuiComponent();

        inline const UpdateFlags getUpdateFlags() const { return (UpdateFlags)p_flags.Get(); }
        inline bool hasFlag(UpdateFlags flag) const { return p_flags.Has(flag); }

        bool enabled = true;
    protected:
        Flags8 p_flags = 0;

        virtual void render(GuiContainer*) {};
        virtual void pre_render(GuiContainer*) {};
        virtual void process_size(GuiContainer*) {};
        virtual void process_position(GuiContainer*) {};
        virtual void process_children(GuiObject* Object, uint32_t index) {};

        inline RectF* getAbsolute(GuiContainer* container) { return &container->m_absolute; }
        inline Color4* getModulate(GuiContainer* container) { return &container->m_modulate; }
        inline RectF* getAbsolute(GuiObject* container) { return &container->m_absolute; }
        inline Color4* getModulate(GuiObject* container) { return &container->m_modulate; }
    private:
        friend class GuiContainer;
    };
}