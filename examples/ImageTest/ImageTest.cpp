#include <ECS/scene.hpp>
#include <ECS/entity.hpp>

#include <ECS/Components/sprite.hpp>

#include <Core/Event.hpp>
#include <Entry.h>

#include <Base/Image.hpp>

struct TextureEntity;

static void mouse(void*, TextureEntity* ent, sre::Event event);

struct TextureEntity : public sreECS::Entity
{
    sre::Image img1{"res://test_texture.png"};
    sre::Image img2{"res://test_texture2.png"};

    sre::Connection button_connection = sre::onEvent.connect(mouse, this);

    TextureEntity()
    {
        sprite.attach(img1.to_sampler());
        sprite.attach(img2.to_sampler());
        sprite.scale = 4;

        setup_components(sprite);
    }
    sreECS::Sprite sprite;
};

static void mouse(void*, TextureEntity* ent, sre::Event event)
{
    switch (event.type())
    {
        case sre::EVENT_MOUSEBUTTON: {
            using namespace sre::events;
            const auto& mbevent = event.get<MouseButton>();
            if (mbevent.pressed && mbevent.button == sre::MB_LEFT)
                ent->sprite.current_frame++;
                ent->sprite.current_frame %= 2;
        } break;
    }
}

#include <Core/Window.hpp>
void sre::initialize()
{
    sre::window_setresizable(true);
    auto texture_palace = new sreECS::Scene;
    texture_palace->add_entity<TextureEntity>();
    texture_palace->make_current();
}