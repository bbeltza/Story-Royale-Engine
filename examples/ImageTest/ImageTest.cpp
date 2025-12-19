#include <Engine.hpp>
#include <ECS.hpp>

#include <Game/Components/Sprite.hpp>

struct TexturePalace;
struct TextureEntity;

struct TexturePalace : public Game::World
{
    TexturePalace()
    {
        auto texture = addEntity<TextureEntity>();
    }
};


static void mouse(void*, TextureEntity* ent, const MouseButton* event);

struct TextureEntity : public Game::Entity
{
    sre::Image img1{"res://test_texture.png"};
    sre::Image img2{"res://test_texture2.png"};

    Texture sprt1{img1};
    Texture sprt2{img2};
    //Texture testsprt;

    ConnectionHandle button_connection = Input::MouseButton.Connect(mouse, this);

    TextureEntity()
    {
        sprite.Attach(sprt1);
        sprite.Attach(sprt2);
        sprite.Scale = { 4, 4 };
        addComponent(sprite);
    }
    Components::Sprite sprite;
};

static void mouse(void*, TextureEntity* ent, const MouseButton* event)
{
    if (event->pressed && event->button == 1)
    ent->sprite.current_frame++;
    ent->sprite.current_frame %= 2;
}

void sre::initialize()
{
    Game::World::setCurrent<TexturePalace>();
}