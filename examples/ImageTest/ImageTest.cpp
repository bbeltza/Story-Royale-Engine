#include <ECS/Scene.hpp>
#include <ECS/Entity.hpp>

#include <ECS/Components/Sprite.hpp>

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
        sprite.reserve_frames(2);
        sprite.add_frame({ 1, 1 }, { 0, 0 }, { 0, 0, 0, 0 }, img1.to_texture());
        sprite.add_frame({ 1, 1 }, { 0, -1 }, { 0, 0, 0, 0 }, img2.to_texture());

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
            if (mbevent.pressed && mbevent.button == sre::MB_LEFT) {
                if (!ent->sprite.next()) {
                    ent->sprite.set(0);
                }
            }
        } break;
    }
}

#include <Core/Window.hpp>
void sre::initialize()
{
    sre::window_set_manualscale(4);
    sre::window_setresizable(true);

    auto texture_palace = new sreECS::Scene;
    texture_palace->add_entity<TextureEntity>();
    sreECS::set_current(texture_palace);
}