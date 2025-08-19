#pragma once
#include "Game/Component.h"

#include "Datatypes/Vector.h"
#include "Datatypes/TimeStamp.h"

namespace Components
{
    class Velocity: public ::Game::Component
    {
        public:
        Velocity() { p_flags = p_pUpdate; }
        Vector2f velocity;
        
        // Normalizes the velocity and multiplies it by the speed parameter
        // @param speed the speed to multiply
        void normalize(float=1.0f);
        void pUpdate(::Game::Entity*, TimeStamp) override;
    };
}