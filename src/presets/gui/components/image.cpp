#include <GUI/Components/image.hpp>
#include <GUI/Components/transform.hpp>
#include <Base/Display.hpp>

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

    Display::DrawTexture(*texture, dimensions, modulate, sre::vec2f::ZERO, DISPLAY_DONT_CENTER);
}