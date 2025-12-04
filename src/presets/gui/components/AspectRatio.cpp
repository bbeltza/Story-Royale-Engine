#include "Game/GuiComponents/AspectRatio.hpp"

void GuiComponents::AspectRatio::process_size(Game::GuiContainer* obj)
{
    getAbsolute(obj)->Size.X = getAbsolute(obj)->Size.Y * ratio;
}