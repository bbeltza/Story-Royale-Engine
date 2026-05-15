#pragma once
#include <ECS/Component.hpp>

#include <Datatypes/Vector.h>
#include <Datatypes/TimeStamp.h>

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