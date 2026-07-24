#include <ECS/Entity.hpp>
#include <ECS/Scene.hpp>

#include <ECS/Components/Sprite.hpp>

using namespace sreECS;

sre::u32 Sprite::add_frames(const Frame* frames, sre::u32 count) {
    sre::u32 i = num_frames();
    m_frames.insert(m_frames.end(), frames, frames+count);
    return i;
}

sre::u32 Sprite::add_frame(Frame&& frame) {
    sre::u32 i = num_frames();
    m_frames.push_back(std::forward<Frame>(frame));
    return i;
}

sre::u32 Sprite::remove_frames(sre::u32 index, sre::u32 count) {
    if (index >= m_frames.size())
        return 0;

    count = (index + count) > m_frames.size() ? static_cast<sre::u32>(m_frames.size() - index) : count;
    {
        auto first = m_frames.begin() + index;
        m_frames.erase(first, first + count);
    }

    return count;
}

void Sprite::on_render(Entity& entity)
{
    if (m_frames.empty())
        return;

    const auto& frame = m_frames.at(m_frame);
    const auto oldoffset = base.offset;
    base.offset += entity.position;
    frame.render(SRE_DRAWFLAG_CAMERA, base, modulate);
    base.offset = oldoffset;
}

sre::RenderInstance1 SpriteFrame::get_renderinstance(const SpriteFrame& base, sre::col4 modulate, sre::Texture*& out_texture) const
{
    out_texture = texture ? texture.get() : base.texture.get();
    if (!out_texture) {
        return {};
    }
    
    sre::vec2i texture_size = out_texture->size();
    sre::vec2f texture_fsize{texture_size};

    sre::rect2Di cur_region = region;
    if (!cur_region.w) {
        cur_region.x = base.region.x;
        cur_region.w = base.region.w;
    }
    if (!cur_region.h) {
        cur_region.y = base.region.y;
        cur_region.h = base.region.h;
    }

    if (cur_region.size.x)
        texture_size.x = cur_region.size.x;
    if (cur_region.size.y)
        texture_size.y = cur_region.size.y;

    sre::vec2ut full_scale = base.scale * scale;
    if (!full_scale.x || !full_scale.y) {
        out_texture = NULL;
        return {};
    }

    return {
        sre::rect2Dut{
            base.offset + offset,
            texture_size * full_scale
        },
        sre::vec2ut::CENTER,
        modulate,
        0,
        sre::vec2f{ cur_region.size.x ? (cur_region.size.x/texture_fsize.x) : 1.0f, cur_region.size.y ? (cur_region.size.y/texture_fsize.y) : 1.0f },
        sre::vec2f{ cur_region.position.x / texture_fsize.x, cur_region.position.y / texture_fsize.y }
    };
}

void SpriteFrame::render(sre::s32 renderflags, const SpriteFrame& base, sre::col4 modulate) const
{
    sre::Texture* cur_texture = NULL;
    sre::RenderInstance1 drawinst = get_renderinstance(base, modulate, cur_texture);
    if (!cur_texture)
        return;

    sre::render::draw1(
        renderflags,
        &drawinst, 1,
        cur_texture
    );
}