#include <Engine.h>
#include <ECS.h>
#include <Game/Components/Sprite.h>
#include <Events/Mouse.h>

struct TexturePalace;
struct TextureEntity;

static size_t frame = 0;

static void mouse(MouseButton* event)
{
    if (event->pressed && event->button == 1)
    frame++;
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
        Engine->Input.mouseButton.Wait();
        auto texture = addEntity<TextureEntity>();
    }
};

struct TextureEntity : public Game::Entity
{
    Texture sprt1;
    Texture sprt2;
    Texture testsprt;

    TextureEntity():
        sprt1("res://test_texture.png"),
        sprt2("res://test_texture2.png"),
        testsprt(10, 10)
    {
        sprite.Attach(sprt1);
        sprite.Attach(sprt2);
        sprite.Attach(testsprt);
        sprite.Scale = { 4, 4 };
        addComponent(&sprite);
    }

    void preRender() override
    {
        sprite.current_frame = frame;
    }
    Components::Sprite sprite;
};