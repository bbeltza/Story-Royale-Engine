#include <ECS/scene.hpp>
#include <ECS/entity.hpp>

#include <ECS/Components/sprite.hpp>

#include <Base/Input.hpp>
#include <Entry.h>

struct TexturePalace;
struct TextureEntity;

struct TexturePalace : public sreECS::World
{
    TextureEntity& texture = add_entity<TextureEntity>();
};


static void mouse(void*, TextureEntity* ent, const MouseButton* event);

struct TextureEntity : public sreECS::Entity
{
    sre::Image img1{"res://test_texture.png"};
    sre::Image img2{"res://test_texture2.png"};

    Texture sprt1{img1};
    Texture sprt2{img2};
    //Texture testsprt;

    ConnectionHandle button_connection = Input::MouseButton.Connect(mouse, this);

    TextureEntity()
    {
        sprite.attach(sprt1);
        sprite.attach(sprt2);
        sprite.scale = { 4, 4 };

        setup_components(sprite);
    }
    sreECS::Sprite sprite;
};

static void mouse(void*, TextureEntity* ent, const MouseButton* event)
{
    if (event->pressed && event->button == 1)
    ent->sprite.current_frame++;
    ent->sprite.current_frame %= 2;
}

void sre::initialize()
{
    auto texture_palace = new TexturePalace;
    texture_palace->make_current();
}