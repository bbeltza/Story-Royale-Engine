#include <ECS/Entity.hpp>
#include <ECS/Scene.hpp>

#include <ECS/Components/Sprite.hpp>

using namespace sreECS;

void Sprite::attach(sre::RSampler&& texture)
{
    textures.push_back(std::move(texture));
}

void Sprite::on_render(Entity& entity)
{
    if (textures.empty()) return;

    auto frame = ut_min(current_frame, textures.size() - 1);
    current_frame = frame;

    auto renderer = sre::get_renderer();
    sre::RSampler& texture = textures[frame];
    sre::vec2i texture_size;
    renderer->sampler_query(texture, &texture_size, NULL);

    if (region.size.x)
        texture_size.x = region.size.x;

    if (region.size.y)
        texture_size.y = region.size.y;

    sre::rect2Dut render_rect(
        entity.position + offset,
        texture_size * scale
    );
    sre::s32 flags = SRE_DRAWFLAGS_USECAM;
    bool flipx = render_rect.size.x < 0;
    bool flipy = render_rect.size.y < 0;
    render_rect.size = render_rect.size.abs();

    sre::vec2f uv{
        region.size.x / texture_size.x * (flipx ? -1.0f : 1.0f),
        region.size.y / texture_size.y * (flipy ? -1.0f : 1.0f)
    };

    renderer->draw1(
        flags,
        {sre::RenderInstance1{
            render_rect,
            sre::vec2ut::CENTER,
            modulate,
            0,
            uv,
            {
                region.position.x / (float)texture_size.x - (flipx ? uv.x : 0.0f),
                region.position.y / (float)texture_size.y - (flipy ? uv.y : 0.0f)
            }
        }},
        {texture.operator sre::Sampler *()}
    );
}

#ifndef IMGUI_DISABLE
    void Sprite::ImGuiUpdate()
    {
        ImGui::DragFloat2("offset", offset);
        ImGui::DragFloat2("scale", scale, 0.1f);
        ImGui::DragInt4("region", region.position);
        ImGui::SliderInt("current_frame", &reinterpret_cast<int&>(current_frame), 0, static_cast<int>(textures.size() - 1));
    
        float v[4] = { modulate.r/255.0f, modulate.g/255.0f, modulate.b/255.0f, modulate.a/255.0f };
        if (ImGui::ColorEdit4("modulate", v))
            modulate = sre::col4::fromNormalized(v[0], v[1], v[2], v[3]);
    }
#endif