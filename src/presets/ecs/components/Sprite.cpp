#include <Base/Draw.hpp>

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

    sre::draw(sre::DDTexture{
        SRE_DRAWFLAGS_USECAM | (render_rect.size.x < 0 ? SRE_DRAWFLAGS_FLIPX : 0) | (render_rect.size.y < 0 ? SRE_DRAWFLAGS_FLIPY : 0),
        { modulate.r, modulate.g, modulate.b, modulate.a },

        render_rect.position.x,
        render_rect.position.y,
        abs(render_rect.size.x),
        abs(render_rect.size.y),

        0.5,
        0.5,

        texture.handle()
    });
}
