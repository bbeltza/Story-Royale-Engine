#include <GameSettings.h>
#include <Engine.h>
#include <ECS.h>
#include <GuiLayer.h>
#include <GuiComponents.h>

static double rot = 0;

struct DisplayText: public Game::GuiLayer
{
    DisplayText()
    {
        auto label = pushGuiObject<Game::GuiObject>();
        label->anchor = Vector2f(0.5, 0);
        label->position = UDim2(0.5, 0, 0, 10);
        label->color = {0, 0, 0, 0};
        
        auto text = label->pushGuiComponent<Game::GuiComponents::UIText>();
        text->color = {255, 255, 255};
        text->text.assign("Hey! This is a Rectangle test, you can move the red rectangle with your mouse, and it should turn green if it touches the white one!\n\nYou can change the size of the rectangle with your mouse wheel");
        text->LoadFontPath("res://fonts/OpenSans-Regular.ttf");
    }

    void postRender();
    void Update(float delta) { rot += delta * 80; }
};

RectF mouseRect(0, 0, 100, 50);
RectI staticRect(0, 20, 250, 90);

void mousewheel(MouseWheel* event)
{
    mouseRect.Size = mouseRect.Size + Vector2f(event->amount * 10);
}

void DisplayText::postRender()
{
    Color4 col;
    if (mouseRect.Intersects(staticRect))
        col = {0, 255, 0, 255}; // Green
    else
        col = {255, 0, 0, 255}; // Red

    Vector2f mPos = Engine->Input.getMouseWorldPosition();
    mPos.Print();
    mouseRect.Position.X = mPos.X;
    mouseRect.Position.Y = mPos.Y;
    Engine->DrawingContext.DrawRectangleAtWorld(staticRect, {255, 255, 255, 255});
    Engine->DrawingContext.DrawRectangleAtWorld(mouseRect, col);
    Engine->DrawingContext.DrawDebug(mouseRect.getTopLeft());
    Engine->DrawingContext.DrawDebug(mouseRect.getTopRight());
    Engine->DrawingContext.DrawDebug(mouseRect.getBottomLeft());
    Engine->DrawingContext.DrawDebug(mouseRect.getBottomRight());
}

#include <File.h>

void Game::Initialize()
{
    printf("%s\n", _game_res);
    GameSettings::ScalingResolution = {320, 180};
    GameSettings::targetFPS = 40;
    GameSettings::Title = "Rectangle Test";

    Game::setGuiLayer<DisplayText>();

    Engine->Input.mouseWheel.Connect(event_callback(mousewheel));
}