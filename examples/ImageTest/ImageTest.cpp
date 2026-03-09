#include <ECS/scene.hpp>
#include <ECS/entity.hpp>

#include <ECS/Components/sprite.hpp>

#include <Core/Event.hpp>
#include <Entry.h>

struct TextureEntity;

static void mouse(void*, TextureEntity* ent, sre::Event event);

struct TextureEntity : public sreECS::Entity
{
    sre::Image img1{"res://test_texture.png"};
    sre::Image img2{"res://test_texture2.png"};

    sre::Texture sprt1{img1};
    sre::Texture sprt2{img2};

    sre::Texture testsprt{ {2, 2}, SDL_PIXELFORMAT_RGBA8888 };

    sre::Connection button_connection = sre::onEvent.connect(mouse, this);

    TextureEntity()
    {
        static const sre::byte testpixels[ 2 * 4*4 ] = {
            255, 150, 100, 50,     255, 255, 255, 255,
            255, 255, 255, 255,     255, 255, 255, 255
        };

        testsprt.update(testpixels, 8);

        sprite.attach(testsprt);
        sprite.attach(sprt1);
        sprite.attach(sprt2);
        sprite.scale = { 4, 4 };

        setup_components(sprite);
    }
    sreECS::Sprite sprite;
};

static void mouse(void*, TextureEntity* ent, sre::Event event)
{
    switch (event.type)
    {
        case sre::EVENT_MOUSEBUTTON:
        if (event.mouse_button.pressed && event.mouse_button.button == sre::MB_LEFT)
            ent->sprite.current_frame++;
            ent->sprite.current_frame %= 2;
        break;
    }
}

void sre::initialize()
{
    auto texture_palace = new sreECS::Scene;
    texture_palace->add_entity<TextureEntity>();
    texture_palace->make_current();
}