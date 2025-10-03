#include "Game/GuiComponents/Image.hpp"
#include "Engine.hpp"

using namespace GuiComponents;

void Image::render(Game::GuiContainer* obj)
{
    if (!CurrentImage) return;

    RectF* abs = getAbsolute(obj);

    Engine->DrawingContext.DrawTexture(*CurrentImage, *abs, Modulate, Vector2f::ZERO);
}