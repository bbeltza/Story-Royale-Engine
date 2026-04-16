#include <GUI/Components/Image.hpp>
#include <GUI/Components/Transform.hpp>
#include <Core/Render.h>

using namespace sreGUI;

void Image::fit(Transform& transform)
{
    if (!texture)
    {
        sre::log<sre::LOGCATEGORY_WARN>("Calling Image::fit on NULL image");
        return;
    }

    sre::RenderInterface* renderer = sre::get_renderer();
    sre::vec2i size;
    renderer->sampler_query(texture, &size, NULL);
    transform.size = sre::udim2::fromoffset(sre::vec2ut{size});
}

void Image::on_render(const sre::rect2Dut &dimensions, sre::RenderInterface* renderer)
{
    if (!texture) return;

    renderer->draw1(
        0, {{
            dimensions,
            sre::vec2ut::ZERO,
            modulate,
            0,
            {1, 1}
        }},
        texture
    );
}