#include <GUI/Components/image.hpp>
#include <GUI/Components/transform.hpp>
#include <Core/Draw.hpp>

using namespace sreGUI;

void Image::fit(Transform& transform)
{
    if (!texture)
    {
        WARN("Calling Image::fit on NULL image");
        return;
    }
    sre::vec2i size = texture->size();
    transform.size = sre::udim2::fromoffset(sre::vec2ut{size});
}

void Image::on_render(const sre::rect2Dut &dimensions)
{
    if (!texture) return;

    sre::draw(sre::DDTexture{
        0,
        modulate,
        dimensions,
        sre::vec2ut::ZERO,

        texture->handle(),
        { 0, 0, 0, 0 }
    });
}