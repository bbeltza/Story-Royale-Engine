#include <ECS/Entity.hpp>
#include <ECS/Scene.hpp>

#include <ECS/Components/Sprite.hpp>

using namespace sreECS;

void Sprite::attach(sre::RAIITexture&& texture)
{
    textures.push_back(std::move(texture));
}

void Sprite::on_render(Entity& entity)
{
    if (textures.empty()) return;

    auto frame = ut_min(current_frame, textures.size() - 1);
    current_frame = frame;

    sre::RAIITexture& texture = textures[frame];
    sre::vec2i texture_size = texture->size();
    
    sre::vec2f texture_fsize{texture_size};
    if (region.size.x)
        texture_size.x = region.size.x;

    if (region.size.y)
        texture_size.y = region.size.y;

    sre::rect2Dut render_rect(
        entity.position + offset,
        texture_size * scale
    );
    sre::s32 flags = SRE_DRAWFLAG_CAMERA;

    sre::render::draw1(
        flags,
        {sre::RenderInstance1{
            render_rect,
            sre::vec2ut::CENTER,
            modulate,
            0,
            { region.size.x ? region.size.x / texture_fsize.x : 1, region.size.y ? region.size.y / texture_fsize.y : 1 },
            { region.position.x / texture_fsize.x, region.position.y / texture_fsize.y }
        }},
        texture.get()
    );
}