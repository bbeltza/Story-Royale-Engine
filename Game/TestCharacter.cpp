#include "ECS.h"
#include "Input.h"
#include "System.h"

#include <assert.h>

class MyCharacter: public Game::Entity
{
public:
    MyCharacter()
    {
        //sys_error("Passed value is not a function!");
        Game::Component* Rect = pushComponent(SHAPE);
        Rect->ShapeComponent.Flags |= CAN_COLLIDE;
        std::cout << this << "\n";
    }
    void Update(float dt) override;

private:
    float v = 250;
};

class BlockThing : public Game::Entity
{
public:
    BlockThing()
    {
        {
            auto c = this->pushComponent(SHAPE);
            c->ShapeComponent.Color = { 40, 20, 70, 255 };
            c->ShapeComponent.xOffset = 50;
            c->ShapeComponent.yOffset = 10;
            c->ShapeComponent.Flags |= CAN_COLLIDE;
        }
        {
            auto c = this->pushComponent(SHAPE);
            c->ShapeComponent.Color = { 50, 90, 40, 255 };
            c->ShapeComponent.xOffset = -25;
            c->ShapeComponent.yOffset = -30;
            c->ShapeComponent.Flags |= CAN_COLLIDE;
        }
    }

    void Update(float dt)
    {
        if (Engine::Input::isKeyPressed(SDL_SCANCODE_A))
            x -= v * dt;
        if (Engine::Input::isKeyPressed(SDL_SCANCODE_D))
            x += v * dt;
        if (Engine::Input::isKeyPressed(SDL_SCANCODE_W))
            y -= v * dt;
        if (Engine::Input::isKeyPressed(SDL_SCANCODE_S))
            y += v * dt;
    }

private:
    int v = 100;
};

EntryWorld::EntryWorld()
{
    auto block = this->addEntity<BlockThing>();
    
    

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

    //getWorld()->CurrentCamera.x = x;
    //getWorld()->CurrentCamera.y = y;

    //std::cout << 1/dt << "\n";
}
