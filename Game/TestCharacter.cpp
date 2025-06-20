#include "ECS.h"
#include "Components.h"
#include "Input.h"
#include "System.h"

#include <assert.h>

class MyCharacter: public Game::Entity
{
public:
    Game::Components::Shape* Rect;

    MyCharacter()
    {
        //sys_error("Passed value is not a function!");
        pushComponentPtr(SHAPE, Rect);
        Rect->flags |= CAN_COLLIDE;
        Rect->shape = CIRCLE;
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

    Vector2i mpos = Engine::Input::getMouseScreenPosition();
    
    if (Rect->isInScreenPoint(mpos))
    {
        Rect->Color = { 128, 128, 128, 255 };
    }
    else
    {
        Rect->Color = { 255, 255, 255, 255 };
    }

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
