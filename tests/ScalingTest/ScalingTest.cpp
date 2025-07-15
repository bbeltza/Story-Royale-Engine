#include <ECS.h>
#include <GameSettings.h>
#include <Engine.h>

void Render()
{
    Vector2f mpos = Engine->Input.getMouseScreenPosition();
    mpos.Print();
    Engine->DrawingContext.DrawRotatedRectangle(RectF(mpos.X, mpos.Y, 10, 10), 45, {255, 255, 255, 255});
}

void Game::Initialize()
{
    GameSettings::ScalingResolution = {320, 180};

    printf("Hello World\n");
    Engine->BeforeRender.Connect(event_callback(Render));
}
