#include "System.h"
#include "ECS.h"

static std::vector<Game::Component*> allocated_components;

Game::Component::Component()
{
    allocated_components.push_back(this);
}

Game::Component::~Component()
{}

void Game::Component::ClearComponents()
{
    while (!allocated_components.empty())
    {
        delete allocated_components.back();
        allocated_components.pop_back();
    }
}