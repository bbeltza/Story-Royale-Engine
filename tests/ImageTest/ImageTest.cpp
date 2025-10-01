#include <Engine.hpp>
#include <ECS.hpp>
#include <Game/Components/Sprite.hpp>
#include <Events/Mouse.hpp>

struct TexturePalace;
struct TextureEntity;

Connection* m_buttonConnection;

struct TexturePalace : public Game::World
{
    TexturePalace()
    {
        auto texture = addEntity<TextureEntity>();
    }
};

struct TextureEntity : public Game::Entity
{
    Texture sprt1;
    Texture sprt2;
    //Texture testsprt;

    TextureEntity():
        sprt1("res://test_texture.png"),
        sprt2("res://test_texture2.png")
    {
        m_buttonConnection->userdata = this;

        sprite.Attach(sprt1);
        sprite.Attach(sprt2);
        sprite.Scale = { 4, 4 };
        addComponent(&sprite);
    }
    Components::Sprite sprite;
};

static void mouse(void*, TextureEntity* ent, MouseButton* event)
{
    if (event->pressed && event->button == 1)
    ent->sprite.current_frame++;
    ent->sprite.current_frame %= 2;
}

void Game::Initialize()
{
    m_buttonConnection = Engine->Input.mouseButton.Connect(mouse, nullptr);
    Game::World::setCurrent<TexturePalace>();
}