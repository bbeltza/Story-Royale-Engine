#include <ECS.h>
#include <GameSettings.h>
#include <Engine.h>

void Render()
{
    Vector2f mpos = Engine->Input.getMouseScreenPosition();
    mpos.PrintLn();
    Engine->DrawingContext.DrawRotatedRectangle(RectF(mpos.X, mpos.Y, 10, 10), 45, {255, 255, 255, 255});
}

void Game::Initialize()
{
    Engine->BeforeRender.Connect(Render, nullptr);
}
