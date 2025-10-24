#include <Engine.hpp>
#include <ECS.hpp>
#include <Classes/TargetTexture.hpp>

void Render()
{
    auto mpos = Input::MouseScreenPosition();
    TargetTexture target;
    Display::DrawRectangle({0, 0, 50, 50}, Color4::WHITE, Color4::WHITE, Vector2f::ZERO);
    Display::DrawRectangle({50, 50, 50, 50}, {255, 0, 0, 128}, Color4::WHITE, Vector2f::ZERO);
    target.Blit({0, 0, 100, 100}, {0, 0, mpos.X, mpos.Y}, Vector2f::ZERO);
}

void Game::Initialize()
{
    Runtime::AfterRender.Connect(Render, nullptr);
    Game::World::setCurrent<Game::World>();
}