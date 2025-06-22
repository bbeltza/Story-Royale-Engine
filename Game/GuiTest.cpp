#include <GuiLayer.h>
#include <GuiComponents.h>

#include <Input.h>

#include <datatypes/Signal.h>

struct testdata
{
    static int count;
};

int testdata::count = 0;

struct TopFrame : public Game::GuiObject
{
    TopFrame()
    {
        color = { 255, 255, 255, 255 };
        position = UDIM2_ZERO;
        size = UDim2(1, 0, 0, 30);
        anchor.X = 0;
        anchor.Y = 0;

        Engine::Input::mouseButton.Connect(onclick);
        Engine::Input::mouseMove.Connect(onmove);

    }

    void Update(float dt) override
    {

    }



    static EVENT_CALLBACK_DECLARE(onmove);
    static EVENT_CALLBACK_DECLARE(onclick);

    Game::GuiComponents::UIText* text = pushGuiComponent<Game::GuiComponents::UIText>();

private:
    bool drag = false;
    GuiObject* parent = (GuiObject*)p_parent;
};

TopFrame* topFrame;

EVENT_CALLBACK(TopFrame::onmove, MouseMove, data, {

        if (!topFrame || !topFrame->drag) return;

topFrame->parent->position.X.Offset += data->delta.X;
topFrame->parent->position.Y.Offset += data->delta.Y;

    })

    EVENT_CALLBACK(TopFrame::onclick, MouseButton, data,
        {
            if (!topFrame) return;

            if (!data->pressed)
            {
                topFrame->drag = false;
                return;
            }
            else if (topFrame->isHovering())
            {
                topFrame->drag = true;
            }

        })

    struct TestFrame : public Game::GuiObject
{
    TestFrame()
    {
        color = { 0, 0, 0, 255 };
        position.X.Scale = 0.5;
        position.Y.Scale = 0.5;
        anchor.X = 0.5;
        anchor.Y = 0.5;
        size = UDim2(0, 200, 0, 150);

        stroke->color = { 255, 255, 255, 255 };

        topFrame = pushGuiObject<TopFrame>();
    }
    ~TestFrame()
    {
        topFrame = nullptr;
    }

    void Update(float dt) override
    {
        //std::cout << isHovering() << "\n";
    }

    Game::GuiComponents::UIStroke* stroke = pushGuiComponent<Game::GuiComponents::UIStroke>();
};



EntryGuiLayer::EntryGuiLayer()
{
    TestFrame* myFrame = this->pushGuiObject<TestFrame>();
}

EntryGuiLayer::~EntryGuiLayer()
{
    std::cout << "Entry gui layer destroyed!\n";
}

void EntryGuiLayer::Update(float dt)
{

}