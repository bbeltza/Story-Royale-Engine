#include <GUI/Components/Text.hpp>

using namespace sreGUI;

void Text::on_render(const sre::rect2Dut& dimensions)
{
    if (!m_fonthandle.valid())
        return;

    sre::FontAtlas& atlas = m_fonthandle.get_atlas();
    atlas.render_text({
        m_str.c_str(),
        -1, 0,
        color,
        NULL // Aimed to exist!
    }, {
        dimensions,
        h_alignment,
        v_alignment,
        count
    });
    //m_font->render(dimensions, color, m_str.c_str(), count, h_alignment, v_alignment);
}