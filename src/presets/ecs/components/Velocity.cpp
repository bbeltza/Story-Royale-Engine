#include "ECS/entity.hpp"
#include "ECS/Components/velocity.hpp"

#include <Engine.hpp>

using namespace sreECS;

void Velocity::normalize(sre::unit speed)
{
    if (speed == 0)
    {
        velocity = velocity.ZERO;
        return;
    }
    
    sre::unit mag = static_cast<sre::unit>(velocity.magnitude());
    if (mag == 0.0) return;

    velocity = velocity / mag * speed;
}

void Velocity::on_pupdate(Entity& e)
{
    e.position += velocity * Runtime::pdelta_time;
}
