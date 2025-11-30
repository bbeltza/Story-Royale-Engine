#include <ECS.hpp>
#include <Base/Display.hpp>
#include <Base/Input.hpp>
#include <Engine.hpp>

void Render()
{
    Vector2f mpos = Input::MouseScreenPosition();
    mpos.PrintLn();
    Display::DrawRotatedRectangle(RectF(mpos.X, mpos.Y, 10, 10), 45, {255, 255, 255, 255});
}

void Game::Initialize()
{
    Runtime::BeforeRender.Connect(Render, nullptr);
}
