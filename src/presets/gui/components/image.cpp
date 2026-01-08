#include <GUI/Components/image.hpp>
#include <GUI/Components/transform.hpp>
#include <Base/Draw.hpp>

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
        { modulate.r, modulate.g, modulate.b, modulate.a },

        dimensions.position.x,
        dimensions.position.y,
        dimensions.size.x,
        dimensions.size.y,
        
        0,
        0,

        texture->handle()
    });
}