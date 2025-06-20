#include "Components.h"

Game::Components::Velocity::Velocity(): 
    velocity(0),
    x_direction(0),
    y_direction(0)
{
    Game::Entity* e = getParent();

    if (!e->velocityComp) e->velocityComp = this;
}

void Game::Components::Velocity::normalize()
{
    const float mg = magnitude();

    if (mg == 1) return;

    x_direction /= mg;
    y_direction /= mg;
}

void Game::Components::Velocity::move(float dt)
{
    auto e = getParent();

    normalize();

    e->x += velocity * x_direction * dt;
    e->y += velocity * y_direction * dt;
}

const float Game::Components::Velocity::magnitude()
{
    return sqrt(x_direction * x_direction + y_direction * y_direction);
}