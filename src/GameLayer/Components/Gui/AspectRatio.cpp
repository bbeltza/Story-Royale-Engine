#include "Game/GuiComponents/AspectRatio.h"

void GuiComponents::AspectRatio::process_size(Game::GuiContainer* obj)
{
    getAbsolute(obj)->Size.X = getAbsolute(obj)->Size.Y * ratio;
}