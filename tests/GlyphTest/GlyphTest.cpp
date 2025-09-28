#include <Engine.h>
#include <Classes/Font.h>

Font* font;
const RectF bounds(10, 10, 120, 30);

void render()
{
    font->Render(bounds, "Hello world guys", -1);
    Engine->DrawingContext.DrawRectangle(bounds, {255, 0, 0, 255}, Color4::WHITE, Vector2f::ZERO, DrawingDevice::dm_Stroke);
}

void Game::Initialize()
{
    static Font _font("res://fonts/OpenSans-Regular.ttf", 12);
    font = &_font;

    Engine->BeforeRender.Connect(render, nullptr);
}