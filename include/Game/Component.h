#pragma once
#include "standard.h"

#include "Datatypes/Flags.h"

namespace Game
{
    class Entity;

    class Component
    {
    public:
    enum ProcessFlags : uint8_t
    {
        p_Special = 0,
        p_Custom = p_Special, // An alias for p_Special
        p_Render = (1 << 0),
        p_Update = (1 << 1),
        p_pUpdate = (1 << 2),
        p_Query = (1 << 3)
    };
    using ComponentProcessFlags = Flags<ProcessFlags>;

    public:
        virtual ~Component() {}
        virtual void Render(Entity *) {}
        virtual void Update(Entity *, delta_model) {}
        virtual void pUpdate(Entity *, delta_model) {}
        virtual bool Query(Entity *) { return false; } // By default it returns false, the derived component should add its own implementation

        inline const ProcessFlags getProcessFlags() const { return p_flags.Get(); }
        inline const bool hasProcessFlag(ProcessFlags flag) const {return p_flags.Has(flag);}

    protected:
        ComponentProcessFlags p_flags;
    };
}