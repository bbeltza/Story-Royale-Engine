#include <Core/Runtime.hpp>
#include <Core/Window.hpp>
#include <Core/Render.h>
#include <Base/File.hpp>
#include <Base/Font.hpp>
#include <Entry.h>

const sre::rect2Dut bounds{
    10, 30, 180, 140
};
static sre::Font opensans_font{};

void render()
{
    sre::FontAtlas& fontatlas = opensans_font.get_atlas();

    sre::render::begin(sre::BLACK, sre::vec2ut::ZERO);
        sre::render::draw2(0, sre::col4{ 255, 0, 0, 100 }, {
            bounds.position,
            bounds.position + sre::vec2ut{bounds.size.x, 0},
            bounds.position + bounds.size,
            bounds.position + sre::vec2ut{0, bounds.size.y}
        }, SRE_PRIMITIVE_LINELOOP);
        fontatlas.render_text(
            sre::FontRenderData{
                "Hello world from sre::FontAtlas::render_text()!!",
                -1, // textlen - `-1` means draw all of the text
                0, // Flags used to render (SRE_DRAWFLAG_CAMERERA(X/Y) can be used)
                sre::WHITE
            },
            sre::FontRenderTextData{
                bounds,
                sre::ALIGN_CENTER,
                sre::ALIGN_CENTER
            },
            NULL, // Unicode set, reserved for future use for unicode support
            NULL
        );
}

void sre::initialize()
{
    {
        sre::File file{"res://fonts/OpenSans-Regular.ttf"};
        sre::Chunk fontchunk = file.allocate();
        opensans_font.load(fontchunk->data, fontchunk->size);
    }

    sre::window_setsize(200, 200);
    sre::beforeRender.connect(::render, nullptr);
}