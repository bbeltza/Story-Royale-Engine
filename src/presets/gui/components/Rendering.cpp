#include "Game/GuiComponents/Stroke.hpp"
#include "Game/GuiComponents/Fill.hpp"

#include "Base/Display.hpp"

void GuiComponents::Fill::render(Game::GuiContainer* obj)
{
    auto absolute = getAbsolute(obj);

    Display::DrawRectangle(*absolute, color * (*getModulate(obj)), sre::vec2f::ZERO, Display::dm_Fill, DISPLAY_DONT_CENTER);
}

void GuiComponents::Stroke::render(Game::GuiContainer* obj)
{
    for (unsigned int i = 0; i < size; i++)
    {
        sre::rect2Dut absolute = *getAbsolute(obj);
        absolute.size.x -= i * 2;
        absolute.size.y -= i * 2;
        absolute.position.x += i;
        absolute.position.y += i;

        Display::DrawRectangle(absolute, color * (*getModulate(obj)), sre::vec2f::ZERO, Display::dm_Stroke, DISPLAY_DONT_CENTER);
    }
}