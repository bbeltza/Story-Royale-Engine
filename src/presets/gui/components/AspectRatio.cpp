#include "Game/GuiComponents/AspectRatio.hpp"

void GuiComponents::AspectRatio::process_size(Game::GuiContainer* obj)
{
    getAbsolute(obj)->size.x = getAbsolute(obj)->size.y * ratio;
}