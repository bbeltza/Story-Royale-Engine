#include <Events/Mouse.hpp>

#include <GUI/object.hpp>
#include <GUI/Components/transform.hpp>
#include <GUI/Components/text.hpp>
#include <GUI/Components/fill.hpp>

#include <Core/Runtime.hpp>
#include <Core/Display.hpp>
#include <Core/Window.hpp>
#include <Core/Input.hpp>
#include <Core/Draw.hpp>

#include <ECS/scene.hpp>

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

sre::DDRect mouseRect{
    SRE_DRAWFLAGS_USECAM,
    { 0, 0, 0, 255 },

    {0, 0, 100, 50},
    sre::vec2ut::CENTER
};

const sre::DDRect staticRect{
    SRE_DRAWFLAGS_USECAM,
    { 255, 255, 255, 255 }, // REMARK: using sre::col4::WHITE causes it to be black as sre::col4::WHITE hasn't been initialized yet
                            // That is a problem...

    {0, 20, 250, 90},
    sre::vec2ut::CENTER
};

void handle_events(void* signal_data, void* connection_data, sre::Event event)
{
    switch (event.type)
    {
        case sre::EVENT_MOUSEWHEEL:
            mouseRect.rect.size += sre::vec2ut{event.mouse_wheel.amount * 10};
            break;
    }
    
}

void DisplayText::post_render()
{
    if (mouseRect.rect.intersects(staticRect.rect))
        mouseRect.color = sre::col4::GREEN; // Green
    else
        mouseRect.color = sre::col4::RED; // Red

    sre::vec2ut mPos = sreECS::mouse_worldcoords();
    mouseRect.rect.position = mPos;
    //mPos.println();

    staticRect.color.println();
    staticRect.rect.println();
    sre::draw(staticRect);
    sre::draw(mouseRect);
    /*
    Display::DrawDebug(mouseRect.top_left());
    Display::DrawDebug(mouseRect.top_right());
    Display::DrawDebug(mouseRect.bottom_left());
    Display::DrawDebug(mouseRect.bottom_right());
    */
}

const int START_WIDTH = 480;
const int START_HEIGHT = 240;
const int START_FRAMERATE = 40;

void setup_settings()
{
    sre::window_setresizable(true);
    sre::window_setsize(START_WIDTH * 2, START_HEIGHT * 2);
    sre::display_autoscale_on(START_WIDTH, START_HEIGHT);
    sre::set_framerate(START_FRAMERATE);
}

void sre::initialize()
{
    setup_settings();

    auto display_text = new DisplayText;
    display_text->set_root();

    sre::onEvent.connect(handle_events, nullptr);
}