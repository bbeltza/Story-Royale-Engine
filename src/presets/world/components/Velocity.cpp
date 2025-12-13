#include "Game/Components/Velocity.hpp"
#include "Game/Entity.hpp"

void Components::Velocity::normalize(float speed)
{
    if (speed == 0)
    {
        velocity = velocity.ZERO;
        return;
    }
    
    auto mag = velocity.magnitude();
    if (mag == 0.0) return;

    velocity = velocity / mag * speed;
}

void Components::Velocity::pUpdate(Game::Entity* e, TimeStamp dt)
{
    e->Position += velocity * dt;
}