#include <GUI/Object.hpp>
#include <GUI/Components/Transform.hpp>
#include <GUI/Components/Text.hpp>
#include <GUI/Components/Fill.hpp>
#include <ECS/Scene.hpp>

#include <Core/Runtime.hpp>
#include <Core/Window.hpp>
#include <Core/Input.hpp>
#include <Core/Render.h>

#include <Base/File.hpp>

#include <Entry.h>

static double rot = 0;

struct DisplayText: public sreGUI::Object
{
    sre::Font font;

    DisplayText()
    {
        {
            sre::File fontfile{"res://fonts/OpenSans-Regular.ttf"};
            sre::Chunk fontchunk = fontfile.allocate();
            font.load(fontchunk->data, fontchunk->size);
        }

        transform.anchor = {0.5, 0}; // NOTE: anchor to be changed to vec2ut after switching to double
        transform.position = {0.5, 0, 0, 10};
        transform.size = { 0.9, 0, 0, 50 };
        
        text.h_alignment = sre::ALIGN_CENTER;
        text.color = {255, 255, 255};
        text.assign("Hey! This is a Rectangle test, you can move the red rectangle with your mouse, and it should turn green if it touches the white one!\n\nYou can resize the rectangle with your mouse wheel");
        text.set_font(font);

        components.setup(transform, text);
    }

    sreGUI::Transform transform;
    sreGUI::Text text;

    void post_render() override;
    void update() override { rot += sre::dt * 80; }
};

sre::RenderInstance1 instances[2] = {
    { {0, 20, 250, 90}, sre::vec2ut::CENTER, sre::WHITE },
    { {0, 0, 100, 50}, sre::vec2ut::CENTER, sre::BLACK }
};

void handle_events(void* signal_data, void* connection_data, sre::Event event)
{
    switch (event.type())
    {
        case sre::EVENT_MOUSEWHEEL:
            using namespace sre::events;
            instances[1].rectangle.size += sre::vec2ut{event.get<MouseWheel>().amount * 10};
            break;
    }
    
}

void DisplayText::post_render()
{
    if (instances[1].rectangle.intersects_from_origin(instances[0].rectangle, sre::vec2ut{-0.5}))
        instances[1].color = sre::GREEN;
    else
        instances[1].color = sre::RED;
        
    sre::vec2ut mPos = sreECS::mouse_worldcoords();
    instances[1].rectangle.position = mPos;

    sre::render::draw1(SRE_DRAWFLAG_CAMERA, instances);
}

const int START_WIDTH = 480;
const int START_HEIGHT = 240;
const int START_FRAMERATE = 40;

void setup_settings()
{
    sre::window_setresizable(true);
    sre::window_setsize(START_WIDTH * 2, START_HEIGHT * 2);
    sre::window_enable_autoscaling(START_WIDTH, START_HEIGHT);
    sre::set_framerate(START_FRAMERATE);
}

void sre::initialize()
{
    setup_settings();

    auto display_text = new DisplayText;
    sreGUI::set_root(display_text);

    sre::onEvent.connect(handle_events, nullptr);
}