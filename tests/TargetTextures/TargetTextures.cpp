#include <Engine.hpp>
#include <ECS.hpp>
#include <Classes/TargetTexture.hpp>

void Render()
{
    auto mpos = Engine->Input.getMouseScreenPosition();
    TargetTexture target;
    Engine->DrawingContext.DrawRectangle({0, 0, 50, 50}, Color4::WHITE, Color4::WHITE, Vector2f::ZERO);
    Engine->DrawingContext.DrawRectangle({50, 50, 50, 50}, {255, 0, 0, 128}, Color4::WHITE, Vector2f::ZERO);
    target.Blit({0, 0, 100, 100}, {0, 0, mpos.X, mpos.Y}, Vector2f::ZERO);
}

void Game::Initialize()
{
    Engine->AfterRender.Connect(Render, nullptr);
    Game::World::setCurrent<Game::World>();
}