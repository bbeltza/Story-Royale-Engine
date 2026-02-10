#include <GUI/Components/Slice.hpp>
#include <Core/Draw.hpp>

using namespace sreGUI;

void Slice::on_render(const sre::rect2Dut &dimensions)
{
    if (!texture)
        return;
    
    const sre::vec2i size = texture->size();
    const sre::vec2i region_corners2 = region_corners * 2;

    sre::DDTexture data{ // Top left corner + texture render data setup!
        0,
        modulate,

        { dimensions.position, {region_corners.x, region_corners.y} },
        sre::vec2ut::ZERO,

        texture->handle(),
        { sre::vec2i::ZERO, region_corners }
    }; 
    sre::draw(data);

    // Top-right corner
    data.region.position.x = size.x - region_corners.x;
    data.rect.position.x = dimensions.position.x + dimensions.size.x;
    data.anchor.x = 1;
    sre::draw(data);
    //
    // Bottom-right corner
    data.region.position.y = size.y - region_corners.y;
    data.rect.position.y = dimensions.position.y + dimensions.size.y;
    data.anchor.y = 1;
    sre::draw(data);
    //
    // Bottom-left corner
    data.region.position.x = 0;
    data.rect.position.x = dimensions.position.x;
    data.anchor.x = 0;
    sre::draw(data); 
    //

    //
    auto check_yregion = [&]()
    {
        if (data.region.size.y >= 0)
            return;
        data.region.size.y = -data.region.size.y;
        data.region.position.y -= data.region.size.y;
    };

    auto check_xregion = [&]()
    {
        if (data.region.size.x >= 0)
            return;
        data.region.size.x = -data.region.size.x;
        data.region.position.x -= data.region.size.x;
    };
    //

    // Edges!!!
    // Left edge
    data.region.position.y = region_corners.y;
    data.region.size.y = size.y - region_corners2.y;
    check_yregion();

    data.rect.position.y -= region_corners.y;
    data.rect.size.y = dimensions.size.y - region_corners2.y;
    sre::draw(data);
    //
    // Right edge
    data.region.position.x = size.x - region_corners.x;
    data.rect.position.x = dimensions.position.x + dimensions.size.x;
    data.anchor.x = 1;
    sre::draw(data);
    //
    // Bottom edge
    data.region.position.x = region_corners.x;
    data.region.position.y = size.y - region_corners.y;
    data.region.size.x = size.x - region_corners2.x;
    check_xregion();

    data.region.size.y = region_corners.y;
    data.rect.position.y = dimensions.position.y + dimensions.size.y;
    data.rect.position.x -= region_corners.x;
    data.rect.size.y = static_cast<sre::unit>(region_corners.y); // "possible loss of data"
    data.rect.size.x = dimensions.size.x - region_corners2.x;
    sre::draw(data);
    //
    // Top edge
    data.region.position.y = 0;
    data.rect.position.y = dimensions.position.y;
    data.anchor.y = 0;
    sre::draw(data);
    //
    // Center
    data.region.position = region_corners;
    data.region.size = size - region_corners2;
    check_xregion();
    check_yregion();

    data.rect.position = dimensions.position + region_corners;
    data.rect.size = dimensions.size - region_corners2;
    data.anchor = sre::vec2ut::ZERO;
    sre::draw(data);
    //
}