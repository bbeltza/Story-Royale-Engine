#include "Game/GuiComponents/Stroke.hpp"
#include "Game/GuiComponents/Fill.hpp"

#include "Base/Display.hpp"

void GuiComponents::Fill::render(Game::GuiContainer* obj)
{
    auto absolute = getAbsolute(obj);



    Display::DrawRectangle(*absolute, color * (*getModulate(obj)), Vector2f::ZERO, Display::dm_Fill, DISPLAY_DONT_CENTER);
}

void GuiComponents::Stroke::render(Game::GuiContainer* obj)
{
    for (unsigned int i = 0; i < size; i++)
    {
        RectF absolute = *getAbsolute(obj);
        absolute.Size.X -= i * 2;
        absolute.Size.Y -= i * 2;
        absolute.Position.X += i;
        absolute.Position.Y += i;

        Display::DrawRectangle(absolute, color * (*getModulate(obj)), Vector2f::ZERO, Display::dm_Stroke, DISPLAY_DONT_CENTER);
    }
}