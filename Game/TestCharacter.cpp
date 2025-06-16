#include "ECS.h"
#include "Components.h"
#include "Input.h"
#include "System.h"

#include <assert.h>

class MyCharacter: public Game::Entity
{
public:
    MyCharacter()
    {
        //sys_error("Passed value is not a function!");
        auto Rect = (Game::Components::Shape*)pushComponent(SHAPE);
        Rect->flags |= CAN_COLLIDE;
        Rect->shape = RECTANGLE;
        pushComponent(VELOCITY);
        velocityComp->velocity = 200;
        std::cout << this << "\n";
    }
    void Update(float dt) override;
};

class BlockThing : public Game::Entity
{
public:
    BlockThing()
    {
        pushComponentPtr(SHAPE, shape);
        shape->flags |= CAN_COLLIDE;
        shape->shape = CIRCLE;
    }

    Game::Components::Shape* shape;

};

EntryWorld::EntryWorld()
{
    auto block = this->addEntity<BlockThing>();

    auto myChar = this->addEntity<MyCharacter>();
}

void MyCharacter::Update(float dt)
{
    velocityComp->x_direction = 0;
    velocityComp->y_direction = 0;

    if (Engine::Input::isKeyPressed(SDL_SCANCODE_LEFT))
        velocityComp->x_direction -= 1;
    if (Engine::Input::isKeyPressed(SDL_SCANCODE_RIGHT))
        velocityComp->x_direction += 1;
    if (Engine::Input::isKeyPressed(SDL_SCANCODE_UP))
        velocityComp->y_direction -= 1;
    if (Engine::Input::isKeyPressed(SDL_SCANCODE_DOWN))
        velocityComp->y_direction += 1;

    //getWorld()->CurrentCamera.x = x;
    //getWorld()->CurrentCamera.y = y;

    //std::cout << 1/dt << "\n";
}
