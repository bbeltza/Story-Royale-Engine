#include "Game/GuiComponents/Image.hpp"
#include "Engine.hpp"

using namespace GuiComponents;

void Image::Fit(Game::GuiObject* obj)
{
    if (!CurrentImage)
    {
        WARN("Calling Image::Fit on NULL image");
        return;
    }
    sre::vec2i size = CurrentImage->size();
    obj->size = sre::udim2::fromoffset(size);
}

void Image::render(Game::GuiContainer* obj)
{
    if (!CurrentImage) return;

    sre::rect2Dut* abs = getAbsolute(obj);

    Display::DrawTexture(*CurrentImage, *abs, Modulate, sre::vec2f::ZERO, DISPLAY_DONT_CENTER);
}