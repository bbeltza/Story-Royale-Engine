#include "ECS.h"
#include "Input.h"

#include <iostream>

class MyCharacter: public Game::Entity
{
public:
    MyCharacter()
    {
        Game::Component* Rect = pushComponent(SHAPE);
        std::cout << this << "\n";
    }
    ~MyCharacter()
    {
        std::cout << "Character destroyed!\n";
    }
    void preRender() override;

private:

};

EntryWorld::EntryWorld()
{
    auto block = this->addEntity<Game::Entity>();
    {
        auto c = block->pushComponent(SHAPE);
    }
    

    auto myChar = this->addEntity<MyCharacter>();
}

void MyCharacter::preRender()
{
    if (Engine::Input::isKeyPressed(SDL_SCANCODE_LEFT))
        x -= 1;
    if (Engine::Input::isKeyPressed(SDL_SCANCODE_RIGHT))
        x += 1;
    if (Engine::Input::isKeyPressed(SDL_SCANCODE_UP))
        y -= 1;
    if (Engine::Input::isKeyPressed(SDL_SCANCODE_DOWN))
        y += 1;

    getWorld()->CurrentCamera.x = x;
    getWorld()->CurrentCamera.y = y;
}
