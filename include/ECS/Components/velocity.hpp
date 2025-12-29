#pragma once
#include <ECS/component.hpp>

#include <datatypes/vector.hpp>
#include <datatypes/timestamp.h>

namespace sreECS
{
    class Velocity: public Component
    {
        public:
        sre::vec2ut velocity;
        
        // Normalizes the velocity
        // @param speed the speed to multiply with
        void normalize(sre::unit speed=1.0);
        void on_pupdate(Entity&) override;
    };
}