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
    Vector2i size = CurrentImage->size();
    obj->size.X.Offset = size.X;
    obj->size.Y.Offset = size.Y;
}

void Image::render(Game::GuiContainer* obj)
{
    if (!CurrentImage) return;

    RectF* abs = getAbsolute(obj);

    Display::DrawTexture(*CurrentImage, *abs, Modulate, Vector2f::ZERO, DISPLAY_DONT_CENTER);
}