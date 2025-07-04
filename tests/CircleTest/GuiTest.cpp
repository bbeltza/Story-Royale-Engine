#include <GuiLayer.h>
#include <GuiComponents.h>

#include <Engine.h>
#include <datatypes/Signal.h>
#include <GuiPresets.h>


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

        text->count = 0;
        text->text.assign("Example text");

        Engine->Input.mouseButton.Connect(onclick);
        Engine->Input.mouseMove.Connect(onmove);

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
            else if (topFrame->isHovering() && data->button == SDL_BUTTON_LEFT)
            {
                topFrame->drag = true;
            }

        })

    static Game::GuiComponents::UIModulate* mod = nullptr;

    struct TestButton : public Game::GuiPresets::GuiButton
{
    TestButton()
    {
        size = UDim2(0, 30, 0, 30);
        anchor.Y = 0.5;
        position.Y.Scale = 0.5;
    }

    void ButtonClick(MouseButton* Event)
    {
        printf("%d\n", Event->button);
        topFrame->text->count = ++count;
    }

    int count = 0;
};

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
        pushGuiObject<TestButton>();

        mod = pushGuiComponent<Game::GuiComponents::UIModulate>();
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

    struct FpsLabel : public Game::GuiObject
    {
        FpsLabel(): text(pushGuiComponent<Game::GuiComponents::UIText>())
        {
            anchor = { 0, 0 };
            position.X = { 0, 10 };
            position.Y = { 0, 10 };
            size.X = {0, 0};
            size.Y = {0, 0};

            text->color = { 255, 255, 255 };
            text->scale = 1;
        }

        void Update(float delta)
        {
            text->text.assign(fmt::format(format_str, 1 / delta));
        }

        std::string format_str = "FPS: {}";
        Game::GuiComponents::UIText* text;

};



EntryGuiLayer::EntryGuiLayer()
{
    auto myFrame = pushGuiObject<TestFrame>();
    auto fpsLabel = pushGuiObject<FpsLabel>();
}

EntryGuiLayer::~EntryGuiLayer()
{
    std::cout << "Entry gui layer destroyed!\n";
}

void EntryGuiLayer::Update(float dt)
{

}