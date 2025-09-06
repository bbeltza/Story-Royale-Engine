#include "Game/Components/Velocity.h"
#include "Game/Entity.h"

void Components::Velocity::normalize(float speed)
{
    if (speed == 0.0f)
    {
        velocity = {0.0f, 0.0f};
        return;
    }
    
    double mag = velocity.getMagnitude();
    if (mag == 0.0) return;

    velocity = velocity / mag * speed;
}

void Components::Velocity::pUpdate(Game::Entity* e, TimeStamp dt)
{
    e->Position += velocity * dt;
}