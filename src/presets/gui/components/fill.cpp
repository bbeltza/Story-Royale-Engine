#include <GUI/Components/Fill.hpp>

#include <Core/Render.h>

using namespace sreGUI;

void Fill::on_render(const sre::rect2Dut& dimensions, sre::RenderInterface* renderer)
{
    renderer->draw1(
        0, {{
            dimensions,
            sre::vec2ut::ZERO,
            color
        }}
    );
}