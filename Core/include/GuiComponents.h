#pragma once

#include "GuiLayer.h"

namespace Game
{
    namespace GuiComponents
    {
        class AspectRatio: public GuiComponent
        {
        public:
            AspectRatio() { p_flags = PROCESS_SIZE; }
            
            float ratio = 1;

            void process_size() override;
        };

        class UIPadding : public GuiComponent
        {
        public:
            UIPadding() { p_flags = PRE_RENDER | PROCESS_POSITION; }

            void setPadding(const UDim& value);

            struct
            {
                UDim left = UDIM_ZERO, right = UDIM_ZERO, top = UDIM_ZERO, bottom = UDIM_ZERO;
            } padding;

            void process_position() override;
            void pre_render() override;

        private:
            SDL_FRect m_oldabs;
        };

        class UIStroke : public GuiComponent
        {
        public:
            UIStroke() { p_flags = RENDER; }

            unsigned int size = 1;
            Color4 color{0, 0, 0, 255};

            void render() override;

        };
        class UIText : public GuiComponent
        {
        public:
            UIText();
            ~UIText();

            const char* text = "Display text!";
            const char* font = "res://fonts/calibri/calibri-regular.ttf";

            unsigned int scale = 0;
            
            Color3 color = {0, 0, 0};

            void render() override;
        
        private:
            TTF_Font* m_font;
        };
            

    }
}