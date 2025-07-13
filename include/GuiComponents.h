#pragma once

#include "GuiLayer.h"

namespace Game
{
    namespace GuiComponents
    {
        class AspectRatio;
        class UIPadding;
        class UIStroke;
        class UIText;
        class UIModulate;

    }
}

namespace Game
{
    namespace GuiComponents
    {
        class AspectRatio : public GuiComponent
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
            Color4 color{ 0, 0, 0, 255 };

            void render() override;

        };
        class UIText : public GuiComponent
        {
        public:
            UIText();
            ~UIText();

            void LoadFontPath(const char* path);

            std::string text = "Display text!";

            unsigned int scale = 0;

            int count = -1;

            Color3 color = { 0, 0, 0 };

            void render() override;

        private:
            TTF_Font* m_font;
            File m_file;
            SDL_Texture* m_cacheTexture = nullptr;
            
            SDL_FRect m_cache_Abs;
            std::string m_cacheText;

            char process_text();
        };
        class UIModulate : public GuiComponent
        {
        public:
            UIModulate() { p_flags = PRE_RENDER; }
            void pre_render() override;

            Color4 Value = { 255, 255, 255, 255 };
        };
    }
}