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
        Rect->ShapeComponent.Shape = CIRCLE;
        pushComponent(VELOCITY);
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
        this->pushComponent(VELOCITY);
        {
            auto c = this->pushComponent(SHAPE);
            c->ShapeComponent.Color = { 40, 20, 70, 255 };
            c->ShapeComponent.xOffset = 50;
            c->ShapeComponent.yOffset = 10;
            c->ShapeComponent.Flags |= CAN_COLLIDE;
            c->ShapeComponent.Shape = CIRCLE;
        }
        {
            auto c = this->pushComponent(SHAPE);
            c->ShapeComponent.Color = { 50, 90, 40, 255 };
            c->ShapeComponent.xOffset = -25;
            c->ShapeComponent.yOffset = -30;
            c->ShapeComponent.Flags |= CAN_COLLIDE;
        }
    }

    /*void Update(float dt)
    {
        velocityComp->VelocityComponent = { 0, 0 };
        if (Engine::Input::isKeyPressed(SDL_SCANCODE_A))
            velocityComp->VelocityComponent.x -= v;
        if (Engine::Input::isKeyPressed(SDL_SCANCODE_D))
            velocityComp->VelocityComponent.x += v;
        if (Engine::Input::isKeyPressed(SDL_SCANCODE_W))
            velocityComp->VelocityComponent.y -= v;
        if (Engine::Input::isKeyPressed(SDL_SCANCODE_S))
            velocityComp->VelocityComponent.y += v;
    }*/

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
    velocityComp->VelocityComponent = { 0, 0 };
    if (Engine::Input::isKeyPressed(SDL_SCANCODE_LEFT))
        velocityComp->VelocityComponent.x -= v;
    if (Engine::Input::isKeyPressed(SDL_SCANCODE_RIGHT))
        velocityComp->VelocityComponent.x += v;
    if (Engine::Input::isKeyPressed(SDL_SCANCODE_UP))
        velocityComp->VelocityComponent.y -= v;
    if (Engine::Input::isKeyPressed(SDL_SCANCODE_DOWN))
        velocityComp->VelocityComponent.y += v;

    //getWorld()->CurrentCamera.x = x;
    //getWorld()->CurrentCamera.y = y;

    std::cout << 1/dt << "\n";
}
