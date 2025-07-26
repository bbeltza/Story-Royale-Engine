#include "Game/Components/Velocity.h"
#include "Game/Entity.h"


void Components::Velocity::pUpdate(Game::Entity* e, delta_model dt)
{
    e->Position += velocity * dt;
}