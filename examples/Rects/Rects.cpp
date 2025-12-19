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
        label->anchor = {0.5, 0}; // NOTE: anchor to be changed to vec2ut after switching to double
        label->position = {0.5, 0, 0, 10};
        label->size = { 1.0, 0, 0, 50 };
        
        text.h_alignment = Font::HCenter;
        text.color = {255, 255, 255};
        text.assign("Hey! This is a Rectangle test, you can move the red rectangle with your mouse, and it should turn green if it touches the white one!\n\nYou can change the size of the rectangle with your mouse wheel");
        text.LoadFont("res://fonts/OpenSans-Regular.ttf");

        label->addComponent(text);
    }

    GuiComponents::Text text;

    void postRender();
    void Update(sre::timeStamp delta) { rot += delta * 80; }
};

sre::rect2Dut mouseRect(0, 0, 100, 50);
sre::rect2Dut staticRect(0, 20, 250, 90);

void mousewheel(void* signal_data, void* connection_data, const MouseWheel* event)
{
    mouseRect.size = mouseRect.size + sre::vec2ut{event->amount * 10};
}

void DisplayText::postRender()
{
    sre::col4 col;
    if (mouseRect.intersects(staticRect))
        col = col.GREEN; // Green
    else
        col = col.RED; // Red

    sre::vec2ut mPos = Input::MouseWorldPosition();
    mPos.println();
    mouseRect.position.x = mPos.x;
    mouseRect.position.y = mPos.y;
    Display::DrawRectangle(staticRect, {255, 255, 255, 255}, sre::vec2f::CENTER, Display::dm_Fill, NULL);
    Display::DrawRectangle(mouseRect, col, sre::vec2f::CENTER, Display::dm_Fill, NULL);
    Display::DrawDebug(mouseRect.top_left());
    Display::DrawDebug(mouseRect.top_right());
    Display::DrawDebug(mouseRect.bottom_left());
    Display::DrawDebug(mouseRect.bottom_right());
}

void sre::initialize()
{
    Game::GuiLayer::setCurrent<DisplayText>();

    Input::MouseWheel.Connect(mousewheel, nullptr);
}