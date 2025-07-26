#pragma once
#include "Game/GuiComponent.h"
#include "Classes/File.h"
#include "SDL.hpp"

namespace GuiComponents
{
    class Text: public ::Game::GuiComponent
    {
    public:
        enum AlignMode
            {
                AlCentered = TTF_WRAPPED_ALIGN_CENTER,
                AlLeft = TTF_WRAPPED_ALIGN_LEFT,
                AlRight = TTF_WRAPPED_ALIGN_RIGHT
            } Alignment;
    public:
        Text();
        ~Text();

        void LoadFont(const char* path);

        std::string text = "Display text!";

        unsigned int scale = 0;
        int count = -1;
        Color3 color = { 0, 0, 0 };
    protected:
        void render(::Game::GuiContainer*) override;
    private:
        File m_file;
    };
}