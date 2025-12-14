#pragma once
#include "Game/Component.hpp"

#include "datatypes/vector.hpp"
#include "datatypes/timestamp.h"

namespace Components
{
    class Velocity: public ::Game::Component
    {
        public:
        sre::vec2ut velocity;
        
        // Normalizes the velocity and multiplies it by the speed parameter
        // @param speed the speed to multiply
        void normalize(float=1.0f);
        void pUpdate(::Game::Entity*, sre::timeStamp) override;
    };
}