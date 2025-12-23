#pragma once
#include <standard>

#include "datatypes/flags.hpp"
#include "datatypes/timestamp.h"

namespace Game
{
    class Entity;

    class Component
    {
    public:
        virtual ~Component() {}
        virtual void Render(Entity *) {}
        //virtual void Update(Entity *, sre::timeStamp) {} // Update() on components isn't implemented yet
        virtual void pUpdate(Entity *, sre::timeStamp) {}
        virtual bool Query(Entity *, float* pt) { return false; } // By default it returns false, the deriving component should add its own implementation
    };
}