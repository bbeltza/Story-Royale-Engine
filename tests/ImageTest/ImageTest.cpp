#include <Engine.h>
#include <ECS.h>
#include <Components.h>

struct TexturePalace;
struct Texture;

static size_t frame = 0;

static void mouse(MouseButton* event)
{
    if (event->pressed && event->button == 1)
    frame = !frame;
}

void Game::Initialize()
{
    printf("Hello world\n");

    Game::setWorld<TexturePalace>();

    Engine->Input.mouseButton.Connect(event_callback(mouse));
}

struct TexturePalace : public Game::World
{
    TexturePalace()
    {
        auto texture = addEntity<Texture>();
    }
};

struct Texture : public Game::Entity
{
    Texture()
    {
        pushComponentPtr(SPRITE, sprite);
        sprite->LoadFile("res://test_texture.png");
        sprite->LoadFile("res://test_texture2.png");
        sprite->Scale = { 3, 3 };
    }

    void preRender() override
    {
        sprite->current_frame = frame;
    }
    Game::Components::Sprite* sprite;
};