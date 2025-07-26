#pragma once
#include "Game/GuiContainer.h"
#include "Game/GuiObject.h"

#include "Datatypes/Flags.h"

namespace Game
{
    class GuiComponent
    {
    public:
        enum UpdateFlags: Flags8::Type
        {
            NOUPDATEFLAGS = 0,
            PROCESS_SIZE = (1 << 1),
            PROCESS_POSITION = (1 << 2),
            PROCESS_CHILDREN = (1 << 3),
            RENDER = (1 << 4),
            PRE_RENDER = (1 << 5),

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