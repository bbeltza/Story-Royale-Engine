#include <Engine.hpp>
#include <ECS.hpp>
#include <GUI.hpp>

#include <Events/Mouse.hpp>
#include <Game/GuiComponents/Text.hpp>

static double rot = 0;

struct DisplayText: public Game::GuiLayer
{
    DisplayText()
    {
        auto label = addChild<Game::GuiObject>();
        label->anchor = Vector2f(0.5, 0);
        label->position = UDim2(0.5, 0, 0, 10);
        
        text.h_alignment = Font::HCenter;
        text.color = {255, 255, 255};
        text.assign("Hey! This is a Rectangle test, you can move the red rectangle with your mouse, and it should turn green if it touches the white one!\n\nYou can change the size of the rectangle with your mouse wheel");
        text.LoadFont("res://fonts/OpenSans-Regular.ttf");

        label->addComponent(text);
    }

    GuiComponents::Text text;

    void postRender();
    void Update(TimeStamp delta) { rot += delta * 80; }
};

RectF mouseRect(0, 0, 100, 50);
RectI staticRect(0, 20, 250, 90);

void mousewheel(void* signal_data, void* connection_data, const MouseWheel* event)
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

    Vector2f mPos = Input::MouseWorldPosition();
    mPos.PrintLn();
    mouseRect.Position.X = mPos.X;
    mouseRect.Position.Y = mPos.Y;
    Display::DrawRectangleAtWorld(staticRect, {255, 255, 255, 255});
    Display::DrawRectangleAtWorld(mouseRect, col);
    Display::DrawDebug(mouseRect.getTopLeft());
    Display::DrawDebug(mouseRect.getTopRight());
    Display::DrawDebug(mouseRect.getBottomLeft());
    Display::DrawDebug(mouseRect.getBottomRight());
}

void Game::Initialize()
{
    Game::GuiLayer::setCurrent<DisplayText>();

    Input::MouseWheel.Connect(mousewheel, nullptr);
}