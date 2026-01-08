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
    sre::s32 flags = SRE_DRAWFLAGS_USECAM | (render_rect.size.x < 0 ? SRE_DRAWFLAGS_FLIPX : 0) | (render_rect.size.y < 0 ? SRE_DRAWFLAGS_FLIPY : 0);
    render_rect.size = render_rect.size.abs();

    sre::draw(sre::DDTexture{
        flags,
        modulate,
        render_rect,
        sre::vec2ut::CENTER,
        texture.handle()
    });
}
