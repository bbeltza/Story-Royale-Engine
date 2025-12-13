#pragma once
#include <standard>

#include "datatypes/Flags.hpp"
#include "datatypes/timestamp.h"

namespace Game
{
    class Entity;

    class Component
    {
    public:
        virtual ~Component() {}
        virtual void Render(Entity *) {}
        //virtual void Update(Entity *, TimeStamp) {} // Update() on components isn't implemented yet
        virtual void pUpdate(Entity *, TimeStamp) {}
        virtual bool Query(Entity *, float* pt) { return false; } // By default it returns false, the deriving component should add its own implementation
    };
}