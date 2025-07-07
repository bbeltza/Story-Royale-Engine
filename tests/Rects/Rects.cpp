#include <GameSettings.h>
#include <Engine.h>
#include <ECS.h>
#include <GuiLayer.h>
#include <GuiComponents.h>

struct DisplayText: public Game::GuiLayer
{
    DisplayText()
    {
        auto label = pushGuiObject<Game::GuiObject>();
        label->anchor = Vector2f(0.5, 0);
        label->position = UDim2(0, 300, 0, 50);
        label->color = {0, 0, 0, 0};
        
        auto text = label->pushGuiComponent<Game::GuiComponents::UIText>();
        text->scale = 1;
        text->color = {255, 255, 255};
        text->text.assign("Hey! This is a Rectangle test, you can move the red rectangle with your mouse, and it should turn green if it touches the white one!");
        #if defined(WIN32)
        text->LoadFontPath("C:/Windows/Fonts/calibri.ttf");
        #elif defined(__linux__)
        text->LoadFontPath("/usr/share/fonts/truetype/liberation/LiberationSerif-Regular.ttf");
        #endif
    }

    void postRender();
};

RectI mouseRect(0, 0, 100, 100);
RectI staticRect(300, 300, 250, 102);

void DisplayText::postRender()
{
    Color4 col;
    if (mouseRect.Intersects(staticRect))
        col = {0, 255, 0, 255}; // Green
    else
        col = {255, 0, 0, 255}; // Red

    Vector2i mPos = Engine->Input.getMouseScreenPosition();
    mouseRect.Position.X = mPos.X;
    mouseRect.Position.Y = mPos.Y;    
    Engine->DrawingContext.DrawRectangle(staticRect, {255, 255, 255, 255});
    Engine->DrawingContext.DrawRectangle(mouseRect, col);
}

void Game::Initialize()
{
    GameSettings::Title = "Rectangle Test";

    Game::setGuiLayer<DisplayText>();
}