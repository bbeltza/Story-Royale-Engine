#include <GuiLayer.h>
#include <GuiComponents.h>

struct TopFrame : public Game::GuiObject
{
    TopFrame()
    {
        color = { 255, 255, 255, 255 };
        position = UDIM2_ZERO;
        size = UDim2(1, 0, 0, 30);
        anchor.X = 0;
        anchor.Y = 0;
    }

    Game::GuiComponents::UIText* text = pushGuiComponent<Game::GuiComponents::UIText>();
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
        size = UDim2(0, 100, 0, 100);

        stroke->color = { 255, 255, 255, 255 };
    }

    void Update(float dt) override
    {
        
    }

    TopFrame* top = pushGuiObject<TopFrame>();
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