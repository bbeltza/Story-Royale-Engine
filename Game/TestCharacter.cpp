#include "ECS.h"
#include "Input.h"
#include "System.h"


class MyCharacter: public Game::Entity
{
public:
    MyCharacter()
    {
        //sys_Error("Passed value is not a function!");

        Game::Component* Rect = pushComponent(SHAPE);
        std::cout << this << "\n";
    }
    ~MyCharacter()
    {
        std::cout << "Character destroyed!\n";
    }
    void Update(float dt) override;

private:
    float v = 250;
};

EntryWorld::EntryWorld()
{
    auto block = this->addEntity<Game::Entity>();
    {
        auto c = block->pushComponent(SHAPE);
    }
    

    auto myChar = this->addEntity<MyCharacter>();
}

void MyCharacter::Update(float dt)
{
    if (Engine::Input::isKeyPressed(SDL_SCANCODE_LEFT))
        x -= v * dt;
    if (Engine::Input::isKeyPressed(SDL_SCANCODE_RIGHT))
        x += v * dt;
    if (Engine::Input::isKeyPressed(SDL_SCANCODE_UP))
        y -= v * dt;
    if (Engine::Input::isKeyPressed(SDL_SCANCODE_DOWN))
        y += v * dt;

    getWorld()->CurrentCamera.x = x;
    getWorld()->CurrentCamera.y = y;

    std::cout << 1/dt << "\n";
}
