#include <iostream>
#include "Window.h"
#include "ECS.h"

Game::World* Game::currentWorld = nullptr;
Game::World* Game::World::s_TargetEntityWorld = nullptr;

Game::World::World()
{
    std::cout << "World created!!\n";
}

Game::World::~World()
{
    std::cout << "World deleted!\n";
    while (!this->m_Entities.empty())
        this->popEntity();
}


void Game::World::popEntity()
{
    delete this->m_Entities.back();
    this->m_Entities.pop_back();
}

void Game::World::removeEntity(unsigned int index)
{
    delete this->m_Entities[index];
    this->m_Entities.erase(m_Entities.begin() + index);
}
void Game::World::removeEntity(void* address)
{
    for (auto i = 0; i < this->m_Entities.size(); i++)
    {
        if (this->m_Entities[i] == address)
        {
            this->removeEntity(i);
            break;
        }
            
    }
}

void Game::World::Render()
{
    for (auto entity : this->m_Entities)
    {
        SDL_Rect r{entity->x - 5, entity->y - 5, 10, 10};

        SDL_SetRenderDrawColor(Engine::Window::sdl_Renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(Engine::Window::sdl_Renderer, &r);
    }
}