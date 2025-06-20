#include "GuiComponents.h"

void Game::GuiComponents::AspectRatio::process_size()
{
    getParentAbs()->w = getParentAbs()->h * ratio;
}