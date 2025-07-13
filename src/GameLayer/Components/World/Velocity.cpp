#include "Components.h"

Game::Components::Velocity::Velocity(): 
    velocity(0),
    x_direction(0),
    y_direction(0)
{
}

void Game::Components::Velocity::normalize()
{
    const float mg = magnitude();

    m_xNormalized = x_direction;
    m_yNormalized = y_direction;

    if (mg == 1) return;

    m_xNormalized /= mg;
    m_yNormalized /= mg;
}

void Game::Components::Velocity::move(Entity* e, float dt)
{
    normalize();

    e->x += velocity * m_xNormalized * dt;
    e->y += velocity * m_yNormalized * dt;
}

const float Game::Components::Velocity::magnitude()
{
    return sqrt(x_direction * x_direction + y_direction * y_direction);
}