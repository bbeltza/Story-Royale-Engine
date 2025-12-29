#include "ECS/entity.hpp" 
#include "ECS/scene.hpp"

#include "ECS/Components/sprite.hpp"

using namespace sreECS;

Sprite::Sprite(): scale(1, 1), offset(0, 0) {}
Sprite::~Sprite()
{
}

void Sprite::attach(Texture& texture)
{
    textures.push_back(&texture);
}

void Sprite::on_render(Entity& entity)
{
    if (textures.empty()) return;

    auto frame = ut_min(current_frame, textures.size() - 1);
    current_frame = frame;
    Texture& texture = *textures[frame];

    sre::rect2Dut render_rect(
        entity.position + offset,
        texture.size() * scale
    );

    Display::DrawTexture(texture, render_rect, modulate, sre::vec2f::CENTER, entity.get_parent());
}
