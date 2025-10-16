#pragma once
#include <standard>

#include "Datatypes/Flags.hpp"
#include "Datatypes/TimeStamp.h"

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
        virtual bool Query(Entity *, float* pt) { return false; } // By default it returns false, the derived component should add its own implementation
    };
}