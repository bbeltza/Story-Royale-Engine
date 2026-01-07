#include <Events/Mouse.hpp>

#include <GUI/object.hpp>
#include <GUI/Components/transform.hpp>
#include <GUI/Components/text.hpp>
#include <GUI/Components/fill.hpp>

#include <Base/Runtime.hpp>
#include <Base/Input.hpp>
#include <Base/Display.hpp>

#include <Entry.h>

static double rot = 0;

struct DisplayText: public sreGUI::Object
{
    DisplayText()
    {
        transform.anchor = {0.5, 0}; // NOTE: anchor to be changed to vec2ut after switching to double
        transform.position = {0.5, 0, 0, 10};
        transform.size = { 0.9, 0, 0, 50 };
        
        text.h_alignment = sre::A_CENTER;
        text.color = {255, 255, 255};
        text.assign("Hey! This is a Rectangle test, you can move the red rectangle with your mouse, and it should turn green if it touches the white one!\n\nYou can resize the rectangle with your mouse wheel");
        text.load("res://fonts/OpenSans-Regular.ttf");

        components.setup(transform, text);
    }

    sreGUI::Transform transform;
    sreGUI::Text text;

    void post_render() override;
    void update() override { rot += sre::dt * 80; }
};

sre::rect2Dut mouseRect(0, 0, 100, 50);
sre::rect2Dut staticRect(0, 20, 250, 90);

void mousewheel(void* signal_data, void* connection_data, const MouseWheel* event)
{
    mouseRect.size = mouseRect.size + sre::vec2ut{event->amount * 10};
}

void DisplayText::post_render()
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
    Display::DrawRectangle(staticRect, {255, 255, 255, 255}, sre::vec2f::CENTER, Display::M_FILL, NULL);
    Display::DrawRectangle(mouseRect, col, sre::vec2f::CENTER, Display::M_FILL, NULL);
    /*
    Display::DrawDebug(mouseRect.top_left());
    Display::DrawDebug(mouseRect.top_right());
    Display::DrawDebug(mouseRect.bottom_left());
    Display::DrawDebug(mouseRect.bottom_right());
    */
}

void sre::initialize()
{
    auto display_text = new DisplayText;
    display_text->set_root();

    Input::MouseWheel.Connect(mousewheel, nullptr);
}