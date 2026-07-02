#include <GUI/Components/Image.hpp>
#include <GUI/Components/Transform.hpp>
#include <Core/Render.h>
#include <Base/Log.h>

using namespace sreGUI;

void Image::fit(Transform& transform)
{
    if (!texture)
    {
        sre::log(SRE_LOG_WARN "Calling Image::fit on NULL image");
        return;
    }

    sre::vec2i size = texture->size();
    transform.size = sre::udim2::fromoffset(sre::vec2ut{size});
}

void Image::on_render(const sre::rect2Dut& dimensions)
{
    sre::vec2f uv{1};
    sre::vec2f uvoffs{0};
    if (texture) {
        sre::vec2f sizef{texture->size()};
        if (region.size.x)
            uv.x = region.size.x / sizef.x;
        if (region.size.y)
            uv.y = region.size.y / sizef.y;
        uvoffs = region.position / sizef;
    }

    // This would in theory draw a plain rectangle if there is no texture attached
    sre::render::draw1(
        0, {{
            dimensions,
            sre::vec2ut::ZERO,
            modulate,
            0,
            uv,
            uvoffs
        }},
        texture.get()
    );
}