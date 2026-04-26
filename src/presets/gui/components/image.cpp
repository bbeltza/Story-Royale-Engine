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

void Image::on_render(const sre::rect2Dut &dimensions)
{
    if (!texture) return;

    sre::render_draw1(
        0, {{
            dimensions,
            sre::vec2ut::ZERO,
            modulate,
            0,
            {1, 1}
        }},
        texture.get()
    );
}