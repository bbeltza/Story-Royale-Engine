#include "Game/Components/Velocity.hpp"
#include "Game/Entity.hpp"

void Components::Velocity::normalize(sre::unit speed)
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

void Components::Velocity::pUpdate(Game::Entity* e, sre::timeStamp dt)
{
    e->Position += velocity * dt;
}