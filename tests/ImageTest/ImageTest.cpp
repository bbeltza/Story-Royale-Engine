#include <Engine.h>
#include <ECS.h>
#include <Game/Components/Sprite.h>
#include <Events/Mouse.h>

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
    Game::World::setCurrent<TexturePalace>();

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
        sprite.LoadFile("res://test_texture.png");
        sprite.LoadFile("res://test_texture2.png");
        sprite.Scale = { 4, 4 };
        addComponent(&sprite);
    }

    void preRender() override
    {
        sprite.current_frame = frame;
    }
    Components::Sprite sprite;
};